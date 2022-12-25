// ************************************************
// Platform-Specific Menu "Extension"
//
// Used for King Kong Xenon
// By Alexandre David (March 2005)
// ************************************************

#include "Precomp.h"

#if defined(_XENON)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "MenuManager.h"

#include "Xenon/VideoManager/VideoManager.h"
#include "Xenon/Demo/DemoLaunchData.h"

#include "INOut/INOjoystick.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#include "SOUnd/Sources/Xenon/xeSND_AudioConsole.h"
#include "XenonGraphics/XeVideoConsole.h"
#include "Xenon/EngineConsole/EngineConsole.h"

// ***********************************************************************************************************************
//    Externs
// ***********************************************************************************************************************

extern WOR_tdst_World*	WOR_gpst_CurrentWorld;
extern BOOL				ENG_gb_ExitApplication;

#if PLAN_B_MEMORY_LEAK_FIX
extern bool				g_bJustRebooted;
#endif

// ***********************************************************************************************************************

namespace MenuManager
{

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

CMenuManager g_MenuManager;

// ***********************************************************************************************************************
//    Class Methods : CMenuManager
// ***********************************************************************************************************************

CMenuManager::CMenuManager()
{
	m_bMustPlay_UbisoftLogo		= FALSE;//popo TRUE
	m_bMustPlay_UniversalLogo	= FALSE;//TRUE
	m_bMustPlay_WingnutLogo		= FALSE;//TRUE
	m_bDisable_AttractMode		= FALSE;
	m_pThread					= NULL;

	m_bHiddenFPS				= FALSE;
	m_bHiddenVersion			= FALSE;
}

// ***********************************************************************************************************************

CMenuManager::~CMenuManager()
{
	Shutdown();
}

// ***********************************************************************************************************************

void CMenuManager::Initialize()
{
	// Error Checking

	if (m_pThread != NULL)
	{
		return;
	}

	// Create Video Manager

	g_pVideoManager = new CVideoManager();

#if defined(DESKTOP_ENABLE)
	// Create Thread

	m_pThread = new CEventThread(eEventID_Count, 16, eThreadID_DebugUpdates);

	m_pThread->SetEventCallback(eEventID_Console_Tick	, fnThreadEvent_Console_Tick);
	m_pThread->SetEventCallback(eEventID_Console_Render	, fnThreadEvent_Console_Render);

	// Initialize Font Manager

	g_FontManager.Initialize();
#endif

	// Create Engine Debugging Console

#if defined(ENGINECONSOLE_ENABLE)
	EngineConsole_Initialize();
#endif
}

// ***********************************************************************************************************************

void CMenuManager::Shutdown()
{
	// Destroy Engine Debugging Console

#if defined(ENGINECONSOLE_ENABLE)
	EngineConsole_Uninit();
#endif

#if defined(DESKTOP_ENABLE)
	// Uninitialize Font Manager

	g_FontManager.Shutdown();

	// Destroy Thread

	if (m_pThread != NULL)
	{
		delete m_pThread;

		m_pThread = NULL;
	}
#endif

	// Destroy Video Manager

	delete g_pVideoManager;
}

// ***********************************************************************************************************************

BOOL CMenuManager::IsInMainMenu()
{
	// Error Checking

	if (WOR_gpst_CurrentWorld == NULL)
	{
		return FALSE;
	}

	// Compare Current World Key To Menu Key

	return (WOR_gpst_CurrentWorld->h_WorldKey == kiWorldKey_MainMenu);
}

// ***********************************************************************************************************************

BOOL CMenuManager::IsInTitleMenu()
{
	// Error Checking

	if (WOR_gpst_CurrentWorld == NULL)
	{
		return FALSE;
	}

	// Compare Current World Key To Menu Key

	return (WOR_gpst_CurrentWorld->h_WorldKey == kiWorldKey_TitleMenu);
}

// ***********************************************************************************************************************

BOOL CMenuManager::IsInDemoMode_InactivityExit()
{
#if _XENON_DEMO_INACTIVITYEXIT
	return TRUE;
#else
	return FALSE;
#endif
}

// ***********************************************************************************************************************

BOOL CMenuManager::IsInDemoMode_HiddenFPS()
{
#if _XENON_DEMO_HIDEFPS
	return TRUE;
#else
	return m_bHiddenFPS ;
#endif
}

// ***********************************************************************************************************************

BOOL CMenuManager::IsInDemoMode_HiddenVersion()
{
#if _XENON_DEMO_HIDEVERSION
	return TRUE;
#else
	return m_bHiddenVersion;
#endif
}


// ***********************************************************************************************************************

BOOL CMenuManager::IsInDemoMode_NoStorageAccess()
{
#if _XENON_DEMO_NOSTORAGEACCESS
	return TRUE;
#else
	return FALSE;
#endif
}

// ***********************************************************************************************************************

BOOL CMenuManager::IsInDemoMode_NoAchievements()
{
#if _XENON_DEMO_NOACHIEVEMENTS
	return TRUE;
#else
	return FALSE;
#endif
}

// ***********************************************************************************************************************

void CMenuManager::DisableAttractMode(BOOL Param_bDisable)
{
	m_bDisable_AttractMode = Param_bDisable;
}

// ***********************************************************************************************************************

void CMenuManager::Tick(float Param_fMSecTimeDelta)
{
	// Main Menu

#if PLAN_B_MEMORY_LEAK_FIX
	if( g_bJustRebooted )
		m_bMustPlay_UbisoftLogo = FALSE;
#endif

	bool bPlayedAllLogos = true;

	static bool bPlayedAttractForKiosk = true;

	if (m_bMustPlay_UbisoftLogo)
	{
		if( !g_pVideoManager->IsPlaying() && !g_pVideoManager->HasRequest() )
		{
			g_pVideoManager->Play("Ubisoft", FALSE, 3000);
			m_bMustPlay_UbisoftLogo = FALSE;
			bPlayedAllLogos = false;
		}
	}
	if (m_bMustPlay_UniversalLogo)
	{
		if( !g_pVideoManager->IsPlaying() && !g_pVideoManager->HasRequest() )
		{
			g_pVideoManager->Play("Universal", FALSE, 3000);
			m_bMustPlay_UniversalLogo = FALSE;
			bPlayedAllLogos = false;
		}
	}
	if (m_bMustPlay_WingnutLogo)
	{
		if( !g_pVideoManager->IsPlaying() && !g_pVideoManager->HasRequest() )
		{
			g_pVideoManager->Play("Wingnut", FALSE, 3000);
			m_bMustPlay_WingnutLogo = FALSE;
			bPlayedAllLogos = false;
			bPlayedAttractForKiosk = false;
		}
	}

	if( bPlayedAllLogos )
	{
		if( !bPlayedAttractForKiosk && CXeDemoLaunchData::GetDemoMode( ) == DEMO_MODE_KIOSK )
		{
			xeINO_SetInactivitySeconds( TimerAttractMode, kfAttractMode_Delay_Seconds );
			bPlayedAttractForKiosk = true;
		}
	}

	if( IsInTitleMenu( ) || IsInMainMenu( ) || CXeDemoLaunchData::GetDemoMode( ) == DEMO_MODE_KIOSK )
	{
		// Attract Mode

		if( !m_bDisable_AttractMode )
		{
			if (xeINO_GetInactivitySeconds( TimerAttractMode ) >= kfAttractMode_Delay_Seconds)
			{
				g_pVideoManager->Play("Attract");
			}
		}
	}

	// Demo Mode - Inactivity Exit

	if (IsInDemoMode_InactivityExit())
	{
		int nTimeout = CXeDemoLaunchData::GetTimeoutValue( );
		if( nTimeout > 0 && xeINO_GetInactivitySeconds( TimerExitDemo ) >= nTimeout )
		{
			CXeDemoLaunchData::ReturnToLauncher();
		}
	}

	// Tick Consoles

#if defined(DESKTOP_ENABLE)
	m_fMSecTimeDelta = Param_fMSecTimeDelta;

	m_pThread->RequestEvent(eEventID_Console_Tick, &m_fMSecTimeDelta);
	m_pThread->WaitForEventComplete(eEventID_Console_Tick);
#endif
}

// ***********************************************************************************************************************

void CMenuManager::RenderConsoles()
{
#if defined(DESKTOP_ENABLE)
	m_pThread->RequestEvent(eEventID_Console_Render);
	m_pThread->WaitForEventComplete(eEventID_Console_Render);
#endif
}

// ***********************************************************************************************************************
//    Thread Callbacks
// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_Console_Tick(int Param_iEventIndex, void* Param_pParameter)
{
	float fMSecTimeDelta = *((float*)Param_pParameter);

	// Audio

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_Tick(fMSecTimeDelta);
#endif

	// Video

#if defined(VIDEOCONSOLE_ENABLE)
	VideoConsole_Tick(fMSecTimeDelta);
#endif

	// Engine

#if defined(ENGINECONSOLE_ENABLE)
	EngineConsole_Tick(fMSecTimeDelta);
#endif
}

// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_Console_Render(int Param_iEventIndex, void* Param_pParameter)
{
	// Audio

#if defined(AUDIOCONSOLE_ENABLE)
	AudioConsole_Render();
#endif

	// Video

#if defined(VIDEOCONSOLE_ENABLE)
	VideoConsole_Render();
#endif

	// Engine

#if defined(ENGINECONSOLE_ENABLE)
	EngineConsole_Render();
#endif
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // _XENON
