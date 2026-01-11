//////////////////////////////////////////////////////////////////////////
// XeSimpleRenderer
// ------------------------
// Simple renderer to take over standard renderer in situations where
// it can not be active (ex: when loading a map)
//////////////////////////////////////////////////////////////////////////

#include "Precomp.h"

#ifdef _XENON

#include "XeSimpleRenderer.h"
#include "XeRenderer.h"

#include "SDK/Sources/Xenon/VideoManager/VideoNoLoop.h"

#include "SDK/Sources/Xenon/Live/Session.h"
#include "SDK/Sources/Xenon/Live/Notifications.h"

#define USE_SIMPLE_RENDERER  1

extern bool g_bHideFPSAndVersionInfo;

extern volatile bool g_bIgnoreShadowClear;

//////////////////////////////////////////////////////////////////////////
// type definitions

enum StateInfo
{
	STATE_NOT_ACTIVE,
	STATE_ACTIVE,
	STATE_WAITING_FOR_DEVICE_RELEASE,
	STATE_DEACTIVATING
};

enum DeviceInfo
{
	DEVICE_NOT_OWNED,
	DEVICE_OWNED
};

enum DeviceCommand
{
	COMMAND_NONE,
	COMMAND_MUST_RELEASE,
	COMMAND_MUST_ACQUIRE
};

//////////////////////////////////////////////////////////////////////////
// 

const char* get_video_filename( int nWorldID )
{
	switch( nWorldID )
	{
		case 1:
		case 2:
			return "";

		case 3:
			return "game:\\Video\\Black.wmv";

		case 100:
			return "game:\\Video\\Intro.wmv";
		case 101:
			return "game:\\Video\\01B.wmv";
		case 201:
			return "game:\\Video\\02A.wmv";
		case 203:
			return "game:\\Video\\02C1.wmv";
		case 204:
			return "game:\\Video\\02C2.wmv";
		case 301:
			return "game:\\Video\\03A.wmv";
		case 302:
			return "game:\\Video\\03B.wmv";
		case 303:
			return "game:\\Video\\03C.wmv";
		case 305:
			return "game:\\Video\\03E.wmv";
		case 306:
			return "game:\\Video\\03F.wmv";
		case 304:
			return "game:\\Video\\03D.wmv";
		case 401:
			return "game:\\Video\\04A.wmv";
		case 402:
			return "game:\\Video\\04A1.wmv";
		case 403:
			return "game:\\Video\\04A2.wmv";
		case 404:
			return "game:\\Video\\04B.wmv";
		case 501:
			return "game:\\Video\\05A1.wmv";
		case 502:
			return "game:\\Video\\05A2.wmv";
		case 504:
			return "game:\\Video\\05C.wmv";
		case 702:
			return "game:\\Video\\07B.wmv";
		case 705:
			return "game:\\Video\\07D.wmv";
		case 901:
			return "game:\\Video\\09B.wmv";
		case 1002:
			return "game:\\Video\\10B.wmv";
		case 1101:
			return "game:\\Video\\11A.wmv";
		case 1200:
			return "game:\\Video\\12A.wmv";
		case 1201:
			return "game:\\Video\\12B.wmv";
		case 703:
			return "game:\\Video\\07B2.wmv";
		case 1401:
			return "game:\\Video\\14A.wmv";
		case 1402:
			return "game:\\Video\\14B.wmv";
		case 1501:
			return "game:\\Video\\15A.wmv";
		case 1701:
			return "game:\\Video\\17A.wmv";
		case 1702:
			return "game:\\Video\\17A2.wmv";
		case 1703:
			return "game:\\Video\\17B.wmv";
		case 1801:
			return "game:\\Video\\18A.wmv";
		case 1803:
			return "game:\\Video\\03CR.wmv";
		case 1804:
			return "game:\\Video\\03BR.wmv";
		case 1805:
			return "game:\\Video\\03AR.wmv";
		case 1901:
			return "game:\\Video\\19A.wmv";
		case 1902:
			return "game:\\Video\\19B.wmv";
		case 2001:
			return "game:\\Video\\20A.wmv";
		case 2003:
			return "game:\\Video\\20C.wmv";
		case 2004:
			return "game:\\Video\\20D.wmv";
		default:
			return "game:\\Video\\Loading.wmv";
	}
}

CVideoNoLoop::PlaybackMode get_video_mode( int nWorld )
{
	if( nWorld == 100 )
		return CVideoNoLoop::PLAY_ALL_ONCE;
	else
		return CVideoNoLoop::PLAY_LOOPING;
}

//////////////////////////////////////////////////////////////////////////
// CXeSimpleRenderer

CXeSimpleRenderer g_oXeSimpleRenderer;

CXeSimpleRenderer::CXeSimpleRenderer( )
	: m_hThread( NULL ),
	  m_dwThreadId( 0 ),
	  m_nState( STATE_NOT_ACTIVE ),
	  m_nDeviceState( DEVICE_NOT_OWNED ),
	  m_nDeviceCommand( COMMAND_NONE ),
	  m_nLoadingWorldID( 0 )
{
}

CXeSimpleRenderer::~CXeSimpleRenderer( )
{
}

void CXeSimpleRenderer::Activate( )
{
#if USE_SIMPLE_RENDERER

	InterlockedExchange( &m_nState, STATE_WAITING_FOR_DEVICE_RELEASE );

    // JFP: Creating the simple renderer thread on processor 2 (first thread of the second core), speeds up load time
    //      by a factor of 3 compared to the performance obtained when it was created on the default processor.
	m_hThread = CreateThread
		(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadEntryPoint,
		NULL,
		CREATE_SUSPENDED,
		&m_dwThreadId
		);

    XSetThreadProcessor( m_hThread, 2 );
    ResumeThread( m_hThread );
#endif
}

void CXeSimpleRenderer::Deactivate( )
{
	InterlockedExchange( &m_nState, STATE_DEACTIVATING );

	WaitForSingleObject( m_hThread, INFINITE );
	CloseHandle( m_hThread );

	m_hThread = NULL;
	m_dwThreadId = 0;
}

bool CXeSimpleRenderer::IsActive( ) const
{
	return m_nState != STATE_NOT_ACTIVE;
}

bool CXeSimpleRenderer::IsReady( ) const
{
	return m_nState == STATE_ACTIVE;
}

bool CXeSimpleRenderer::IsDeactivating( ) const
{
	return m_nState == STATE_DEACTIVATING;
}

void CXeSimpleRenderer::SignalMainRenderingOff( )
{
	InterlockedExchange( &m_nState, STATE_ACTIVE );
}

void CXeSimpleRenderer::SignalDeactivated( )
{
	InterlockedExchange( &m_nState, STATE_NOT_ACTIVE );
}

DWORD CXeSimpleRenderer::ThreadEntryPoint(void* pData)
{
	// hide FPS
    bool bPreviousHideFPSAndVersionInfo = g_bHideFPSAndVersionInfo;
	g_bHideFPSAndVersionInfo = true;

#if defined(_XENON_RENDERER_USETHREAD)

	// enable rendering by acquiring thread ownership
	g_oXeRenderer.AcquireThreadOwnership( );

	g_oXeSimpleRenderer.m_nDeviceState = DEVICE_OWNED;

	// open video file
	CVideoNoLoop video;
	video.Open( get_video_filename( g_oXeSimpleRenderer.m_nLoadingWorldID ) );
	video.SetMode( get_video_mode( g_oXeSimpleRenderer.m_nLoadingWorldID ) );

	g_oXeRenderer.SetVideo( &video );

	// Backup presentation interval
	DWORD dwOldValue;
	g_oXeRenderer.GetDevice()->GetRenderState( D3DRS_PRESENTINTERVAL, &dwOldValue );

	// Set new presentation interval
	g_oXeRenderer.GetDevice()->SetRenderState( D3DRS_PRESENTINTERVAL, D3DPRESENT_INTERVAL_ONE );

	// display until deactivating
	while( !g_oXeSimpleRenderer.IsDeactivating( ) || !video.IsDone( ) )
	{
		// release device if requested
		if( g_oXeSimpleRenderer.m_nDeviceCommand == COMMAND_MUST_RELEASE )
		{
			if( g_oXeSimpleRenderer.m_nDeviceState == DEVICE_OWNED )
				g_oXeRenderer.ReleaseThreadOwnership( );

			g_oXeSimpleRenderer.m_nDeviceState = DEVICE_NOT_OWNED;
			g_oXeSimpleRenderer.m_nDeviceCommand = COMMAND_NONE;
		}

		// acquire device if requested
		if( g_oXeSimpleRenderer.m_nDeviceCommand == COMMAND_MUST_ACQUIRE )
		{
			if( g_oXeSimpleRenderer.m_nDeviceState == DEVICE_NOT_OWNED )
				g_oXeRenderer.AcquireThreadOwnership( );

			g_oXeSimpleRenderer.m_nDeviceState = DEVICE_OWNED;
			g_oXeSimpleRenderer.m_nDeviceCommand = COMMAND_NONE;
		}

		// draw something
		if( g_oXeSimpleRenderer.m_nDeviceState == DEVICE_OWNED )
		{
			g_oXeRenderer.ClearTarget( 0 );
			g_bIgnoreShadowClear = true;
			g_oXeRenderer.PresentFrame( );
			g_bIgnoreShadowClear = false;
		}

		g_XeNotificationManager.CheckForNotifications( );
		//g_XeLiveSession.UpdateSession( );
	}

	video.Close( );
	g_oXeRenderer.SetVideo( NULL );

	// Restore presentation interval
	g_oXeRenderer.GetDevice()->SetRenderState( D3DRS_PRESENTINTERVAL, dwOldValue );

	g_oXeRenderer.ReleaseThreadOwnership();
	g_oXeSimpleRenderer.SignalDeactivated( );

#else

	while( !g_oXeSimpleRenderer.IsDeactivating( ) )
	{
		// release device if requested
		if( g_oXeSimpleRenderer.m_nDeviceCommand == COMMAND_MUST_RELEASE )
		{
			g_oXeSimpleRenderer.m_nDeviceState = DEVICE_NOT_OWNED;
			g_oXeSimpleRenderer.m_nDeviceCommand = COMMAND_NONE;
		}

		// acquire device if requested
		if( g_oXeSimpleRenderer.m_nDeviceCommand == COMMAND_MUST_ACQUIRE )
		{
			g_oXeSimpleRenderer.m_nDeviceState = DEVICE_OWNED;
			g_oXeSimpleRenderer.m_nDeviceCommand = COMMAND_NONE;
		}

		// give processing time to other threads since we don't need it
		Sleep( 50 );

		g_XeNotificationManager.CheckForNotifications( );
	}

	g_oXeSimpleRenderer.SignalDeactivated( );

#endif

	// show FPS
	g_bHideFPSAndVersionInfo = bPreviousHideFPSAndVersionInfo;

	return 0;
}

bool CXeSimpleRenderer::OwnsDevice( ) const
{
	return m_nDeviceState == DEVICE_OWNED;
}

void CXeSimpleRenderer::RequestReleaseDeviceOwnership( )
{
	InterlockedExchange( &m_nDeviceCommand, COMMAND_MUST_RELEASE );
}

void CXeSimpleRenderer::SignalDeviceOwnershipAvailable( )
{
    InterlockedExchange( &m_nDeviceCommand, COMMAND_MUST_ACQUIRE );
}

int CXeSimpleRenderer::GetLoadingWorldID( )
{
	return m_nLoadingWorldID;
}

void CXeSimpleRenderer::SetLoadingWorldID( int nWorldID )
{
	m_nLoadingWorldID = nWorldID;
}

#endif // _XENON
