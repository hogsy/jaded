// ************************************************
// Hack Management
//
// Used for King Kong Xenon Debugging
// By Alexandre David (February 2005)
// ************************************************

#ifndef __HackManager_h__
#define __HackManager_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "HackManager_Native.h"

// ***********************************************************************************************************************

namespace HackManager
{

// ***********************************************************************************************************************
//    Configuration
// ***********************************************************************************************************************

// - Forces all hacks to "inactive" status (commented-out by default)

//#define HACKMANAGER_DISABLEALL

// ***********************************************************************************************************************

// - Asserts on all active hack requests (commented-out by default)

//#define HACKMANAGER_ASSERTONREQUEST

// ***********************************************************************************************************************

// - Allows Hack Manager to store a name string for each hack (for logging purposes)

#if !defined(_FINAL_)
#define HACKMANAGER_STORE_NAMES
#endif

// ***********************************************************************************************************************

// - Allows Hack Manager to check parameter values (for debugging)

#if defined(_DEBUG)
#define HACKMANAGER_ERRORCHECKING
#endif

// ***********************************************************************************************************************
//    Defines
// ***********************************************************************************************************************

#define kHackManager_PlatformFlag_Unknown		0x00
#define kHackManager_PlatformFlag_Editor		0x01
#define kHackManager_PlatformFlag_PlayStation2	0x02
#define kHackManager_PlatformFlag_GameCube		0x04
#define kHackManager_PlatformFlag_PC			0x08
#define kHackManager_PlatformFlag_Xbox			0x10
#define kHackManager_PlatformFlag_Xenon			0x20

// ***********************************************************************************************************************
//    Macros - External
// ***********************************************************************************************************************

#if !defined(HACKMANAGER_DISABLEALL)
#define HackManager_IsActive(eHackID) g_HackManager.IsActive(eHackID)
#else
#define HackManager_IsActive(eHackID) FALSE
#endif

// ***********************************************************************************************************************
//    Macros - Internal
// ***********************************************************************************************************************

#if defined(HACKMANAGER_STORE_NAMES)
#define SetName(strName) strName,
#else
#define SetName(strName)
#endif

// ***********************************************************************************************************************

#if !defined(HACKMANAGER_DISABLEALL)
#define SetActive(bIsActive) bIsActive,
#else
#define SetActive(bIsActive)
#endif

// ***********************************************************************************************************************

#define SetPlatforms(iPlatformFlags) iPlatformFlags

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
#if defined(HACKMANAGER_STORE_NAMES)
	char*	pName;
#endif
#if !defined(HACKMANAGER_DISABLEALL)
	BOOL	bIsActive;
#endif
	int		iPlatformFlags;
}
SHackDescriptor;

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	// --------------------------------------------------------------------------------

	eHack_Placeholder,

	// <-- Insert New Entries Here

	// --------------------------------------------------------------------------------

	eHack_Count
}
eHack_List;

// ***********************************************************************************************************************
//    Hack Descriptors
// ***********************************************************************************************************************

// - Create Platform Shortcuts

#define HK_EDI		kHackManager_PlatformFlag_Editor
#define HK_PS2		kHackManager_PlatformFlag_PlayStation2
#define HK_GC		kHackManager_PlatformFlag_GameCube
#define HK_PC		kHackManager_PlatformFlag_PC
#define HK_XBOX		kHackManager_PlatformFlag_Xbox
#define HK_XENON	kHackManager_PlatformFlag_Xenon

// - Entry List

const SHackDescriptor Hacks[eHack_Count] =
{
	{
		// --------------------------------------------------------------------------------
		// --------------------------------------------------------------------------------
		// [Hack Name      ] Xenon Placeholder Hack
		// [Author         ] Alexandre David
		// [Date           ] March 7th, 2005
		// [Game - Platform] King Kong - Xenon

		// [Source File(s) ] HackManager.h (indicate here files modified by the hack)

		// [Description    ] Quick description.
		// [               ] Try to explain why the hack is there.

		// [Comments       ] Comments can be used to explain on what condition the hack
		// [               ] could be removed, since it's usually something to strive for.
		// --------------------------------------------------------------------------------

		SetName("Xenon PlaceHolder Hack")
		SetActive(FALSE) // Your hacks will most like have Active set to TRUE
		SetPlatforms(HK_XENON)

		// --------------------------------------------------------------------------------
		// --------------------------------------------------------------------------------
	},

	// <-- Insert New Entries Here

};

// - Discard Platform Shortcuts

#undef HK_EDI
#undef HK_PS2
#undef HK_GC
#undef HK_PC
#undef HK_XBOX
#undef HK_XENON

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CHackManager
{
private:
	int m_iPlatformFlags;

public:
    M_DeclareOperatorNewAndDelete();

	CHackManager();
	~CHackManager();

	BOOL IsActive(eHack_List Param_eHackID);
};

// ***********************************************************************************************************************
//    Prototypes - Native
// ***********************************************************************************************************************

void Assert();

// ***********************************************************************************************************************
//    Global Variables
// ***********************************************************************************************************************

extern CHackManager g_HackManager;

// ***********************************************************************************************************************

} // Namespace

using namespace HackManager;

// ***********************************************************************************************************************

#endif // __HackManager_h__
