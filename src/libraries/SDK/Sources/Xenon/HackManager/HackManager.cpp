// ************************************************
// Hack Management
//
// Used for King Kong Xenon Debugging
// By Alexandre David (February 2005)
// ************************************************

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "Precomp.h"

#include "HackManager.h"

// ***********************************************************************************************************************

namespace HackManager
{

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

CHackManager g_HackManager;

// ***********************************************************************************************************************
//    Class Methods : CHackManager
// ***********************************************************************************************************************

CHackManager::CHackManager()
{
	// Detect Platform

	m_iPlatformFlags = kHackManager_PlatformFlag_Unknown;

#if defined(ACTIVE_EDITORS)
	m_iPlatformFlags |= kHackManager_PlatformFlag_Editor;
#endif

#if defined(PSX2_TARGET)
	m_iPlatformFlags |= kHackManager_PlatformFlag_PlayStation2;
#endif

#if defined(_GAMECUBE)
	m_iPlatformFlags |= kHackManager_PlatformFlag_GameCube;
#endif

#if defined(PCWIN_TOOL) || defined(_PC_RETAIL)
	m_iPlatformFlags |= kHackManager_PlatformFlag_PC;
#endif

#if defined(_XBOX)
	m_iPlatformFlags |= kHackManager_PlatformFlag_Xbox;
#endif

#if defined(_XENON)
	m_iPlatformFlags |= kHackManager_PlatformFlag_Xenon;
#endif
}

// ***********************************************************************************************************************

CHackManager::~CHackManager()
{
}

// ***********************************************************************************************************************

BOOL CHackManager::IsActive(eHack_List Param_eHackID)
{
#if defined(HACKMANAGER_DISABLEALL)
	return FALSE;
#else

#if defined(HACKMANAGER_ERRORCHECKING)
	if ((Param_eHackID < 0) || (Param_eHackID >= eHack_Count))
	{
		Assert();
		return FALSE;
	}
#endif

	if ((Hacks[Param_eHackID].iPlatformFlags & m_iPlatformFlags) && Hacks[Param_eHackID].bIsActive)
	{
#if defined(HACKMANAGER_ASSERTONREQUEST)
		Assert();
#endif
		return TRUE;
	}

	return FALSE;
#endif
}

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************
