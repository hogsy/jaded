// ************************************************
// Platform-Specific Menu "Extension"
//
// Used for King Kong Xenon
// By Alexandre David (March 2005)
// ************************************************

#ifndef __MenuManager_h__
#define __MenuManager_h__

#if defined(_XENON)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Xenon/EventThread/EventThread.h"

// ***********************************************************************************************************************

namespace MenuManager
{

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const float		kfAttractMode_Delay_Seconds		= 60.0f;
const BIG_KEY	kiWorldKey_MainMenu				= 0x3d00c45a;
const BIG_KEY	kiWorldKey_TitleMenu			= 0x3d00c456;

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eEventID_Console_Tick,
	eEventID_Console_Render,

	eEventID_Count
}
eEvents_MenuManager;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CMenuManager
{
private:
	CEventThread*	m_pThread;
	BOOL			m_bMustPlay_UbisoftLogo;
	BOOL			m_bMustPlay_UniversalLogo;
	BOOL			m_bMustPlay_WingnutLogo;
	BOOL			m_bDisable_AttractMode;
	float			m_fMSecTimeDelta;

	BOOL			m_bHiddenVersion;
	BOOL			m_bHiddenFPS;

public:
    M_DeclareOperatorNewAndDelete();

	CMenuManager();
	~CMenuManager();

	void	Initialize();
	void	Shutdown();

	BOOL	IsInMainMenu();
	BOOL	IsInTitleMenu();
	BOOL	IsInDemoMode_InactivityExit();
	BOOL	IsInDemoMode_HiddenFPS();
	BOOL	IsInDemoMode_HiddenVersion();
	BOOL	IsInDemoMode_NoStorageAccess();
	BOOL	IsInDemoMode_NoAchievements();

	void	SetDemoMode_HiddenFPS(BOOL bHiddenFPS) { m_bHiddenFPS = bHiddenFPS ; }
	void	SetDemoMode_HiddenVersion(BOOL bHiddenVersion) { m_bHiddenVersion = bHiddenVersion ; }

	void	DisableAttractMode(BOOL Param_bDisable);

	void	Tick(float Param_fMSecTimeDelta);
	void	RenderConsoles();
};

// ***********************************************************************************************************************
//    Thread Callbacks
// ***********************************************************************************************************************

void CALLBACK fnThreadEvent_Console_Tick(int Param_iEventIndex, void* Param_pParameter);
void CALLBACK fnThreadEvent_Console_Render(int Param_iEventIndex, void* Param_pParameter);

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

extern __declspec(align(32)) CMenuManager g_MenuManager;

// ***********************************************************************************************************************

} // Namespace

using namespace MenuManager;

// ***********************************************************************************************************************

#endif // _XENON

#endif // __MenuManager_h__
