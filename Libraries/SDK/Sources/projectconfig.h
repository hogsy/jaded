// ************************************************
// Project Config
// General master file to be used to enable or
// disable properties and settings

// Alexandre David (Dec 15th 2004)
// ************************************************

#pragma once

//#define JADEFUSION

// HACK for NY demo
#define PLAN_B_MEMORY_LEAK_FIX   0

// ***********************************************************************************************************************
//    Feature Defines
// ***********************************************************************************************************************

// JFP: A define to use for Montreal specific features. To be used on editor features that are only to be used in Montreal for instance.
//		This define must be set to 0 for all compilations done by the Montpellier team.
//#define MONTREAL_SPECIFIC 1
#if MONTREAL_SPECIFIC
#pragma message("**** MONTREAL_SPECIFIC is defined")

#define JADE_KEY_SERVER
#endif // MONTREAL_SPECIFIC

// NOTE: _FINAL_ is the official symbol to use. However, some people were using _FINAL,
// so for safety reasons, we define it here too.  Please use _FINAL_ in new code.
#ifdef _FINAL_
#	ifndef _FINAL
#		define _FINAL
#	endif
#endif

// ***********************************************************************************************************************
//    Demo Mode Features
// ***********************************************************************************************************************

#if !defined(_XENON_DEMO)
#	define _XENON_DEMO 0   // Set to 1 to activate all other _XENON_DEMO defines at once
#endif

#if _XENON_DEMO
#	define _XENON_DEMO_INACTIVITYEXIT   1
#	define _XENON_DEMO_HIDEFPS          1
#	define _XENON_DEMO_HIDEVERSION		1
#	define _XENON_DEMO_NOSTORAGEACCESS  1
#	define _XENON_DEMO_NOACHIEVEMENTS   1
#endif

// Automatic Exit When Controller Inactive

#if !defined(_XENON_DEMO_INACTIVITYEXIT)
#   define _XENON_DEMO_INACTIVITYEXIT 0
#endif

#if _XENON_DEMO_INACTIVITYEXIT
#pragma message("**** Demo Mode - Inactivity Exit")
#endif

// Hide FrameRate Display

#if !defined(_XENON_DEMO_HIDEFPS)
// JFP: Activating for New York press tour. (October 2005)
#   define _XENON_DEMO_HIDEFPS 0
#endif

#if _XENON_DEMO_HIDEFPS
#pragma message("**** Demo Mode - Hidden FPS")
#endif

// Hide Version Display

#if !defined(_XENON_DEMO_HIDEVERSION)
#	define _XENON_DEMO_HIDEVERSION 0
#endif

#if _XENON_DEMO_HIDEVERSION
#pragma message("**** Demo Mode - Hidden Version")
#endif

// Block Storage Access

#if !defined(_XENON_DEMO_NOSTORAGEACCESS)
#   define _XENON_DEMO_NOSTORAGEACCESS 0
#endif

#if _XENON_DEMO_NOSTORAGEACCESS
#pragma message("**** Demo Mode - No Storage Access")
#endif

// Block Achievements

#if !defined(_XENON_DEMO_NOACHIEVEMENTS)
#   define _XENON_DEMO_NOACHIEVEMENTS 0
#endif

#if _XENON_DEMO_NOACHIEVEMENTS
#pragma message("**** Demo Mode - No Achievements")
#endif

// ***********************************************************************************************************************
//    QA Feature
// ***********************************************************************************************************************

#if !defined(_FINAL_)
#define JOYRECORDER
#pragma message("**** JoyRecorder - Enabled")
#endif

#if !defined(_FINAL_)
#define XENONVIDEOSTATISTICS
#pragma message("**** Xenon Video Statistics - Enabled")
#endif

// ***********************************************************************************************************************
//    PIX, PROFILING and other things
// ***********************************************************************************************************************

#if defined(_XENON) && !defined(_FINAL_)
#define TRIGGER_ENABLE
#pragma message("**** Trigger - Enabled ****")
#endif

#if defined(_XENON) && !defined(_FINAL_) && defined(_PROFILE)
#define PROFILE_ENABLE
#pragma message("**** Profiling - Enabled ****")
#endif

#if defined(_XENON) && !defined(_FINAL_)
#define PIX_ENABLE
#define USE_PIX
#pragma message("**** PIX - Enabled ****")
#endif

// ***********************************************************************************************************************
//    Sound
// ***********************************************************************************************************************

#if !defined(_XENON_SOUND_ENGINE) && defined(_XENON)
#	define _XENON_SOUND_ENGINE				1
#endif

#if !defined(_XENON_SOUND_FX) && defined(_XENON)
#	define _XENON_SOUND_FX					1
#endif

#if !defined(_XENON_SOUND_FORCEALTERNATEBF) && defined(_XENON)
#	define _XENON_SOUND_FORCEALTERNATEBF	1
#endif

// ***********************************************************************************************************************
//    Threading Configuration
// ***********************************************************************************************************************

#if !defined(_XENON_THREAD_ALLOWSETNAME)
#	define _XENON_THREAD_ALLOWSETNAME	0
#endif

// ***********************************************************************************************************************
//    Debug
// ***********************************************************************************************************************

#if !defined(_XENON_DEBUG_SAFEMODE)
#   define _XENON_DEBUG_SAFEMODE 0
#endif

#if _XENON_DEBUG_SAFEMODE
#pragma message("**** Debug - Safe Mode")
#endif

// ***********************************************************************************************************************
//    Desktop
// ***********************************************************************************************************************

#if defined(_XENON) && !defined(_FINAL_)
#define DESKTOP_ENABLE
#pragma message("**** Desktop - Base Enabled")
#endif

#if defined(DESKTOP_ENABLE) && _XENON_SOUND_ENGINE
#define AUDIOCONSOLE_ENABLE
#pragma message("**** Desktop - AudioConsole Enabled")
#endif

#if defined(DESKTOP_ENABLE)
#define VIDEOCONSOLE_ENABLE
#pragma message("**** Desktop - VideoConsole Enabled")
#endif

#if defined(DESKTOP_ENABLE)
#define ENGINECONSOLE_ENABLE
#pragma message("**** Desktop - EngineConsole Enabled")
#endif

// ***********************************************************************************************************************
//    Global Includes
// ***********************************************************************************************************************

#if defined(_XENON)
#   if !defined(_XBOX)
#       define _XBOX // Temporary Fix For Microsoft's SDK not checking for a Xenon define
#   endif // !defined(_XBOX)
#   include <xtl.h>
#   include <xaudio.h>
#   undef _XBOX // Temporary Fix For Microsoft's SDK not checking for a Xenon define
#endif

// ***********************************************************************************************************************
//    Rendering Defines
// ***********************************************************************************************************************

#if defined(_XENON_RENDER)

#   if defined(_XENON)
#       pragma message("ProjectConfig: _XENON_RENDER for Xenon")
#       if !defined(_XENON_RENDERER_USETHREAD)
#           define _XENON_RENDERER_USETHREAD
#       endif
#   else // -> !defined(_XENON)
#       pragma message("ProjectConfig: _XENON_RENDER for PC - DirectX 9.0 - Defining _XENON_RENDER_PC")
#       if !defined(_XENON_RENDER_PC)
#           define _XENON_RENDER_PC
#       endif
#   endif // defined(_XENON)

// SC: Enable|Disable vertex compression
#   pragma message("ProjectConfig: _XE_COMPRESS_VERTEX - Enabling vertex compression")
#   if !defined(_XE_COMPRESS_VERTEX)
#       define _XE_COMPRESS_VERTEX
#   endif

#   if defined(_XE_COMPRESS_VERTEX)
#       if defined(_XENON) // SC: Xenon only because GeForce 6 are not supporting DEC3N
#           pragma message("ProjectConfig: _XE_COMPRESS_NORMALS - Normals will be stored using DEC3N")
#           if !defined(_XE_COMPRESS_NORMALS)
#               define _XE_COMPRESS_NORMALS
#           endif
#       endif
#       pragma message("ProjectConfig: _XE_COMPRESS_TANGENTS - Tangents will be stored using SHORT4N")
#       if !defined(_XE_COMPRESS_TANGENTS)
#           define _XE_COMPRESS_TANGENTS
#       endif
#       pragma message("ProjectConfig: _XE_COMPRESS_WEIGHTS - BlendWeights will be stored using SHORT4N")
#       if !defined(_XE_COMPRESS_WEIGHTS)
#           define _XE_COMPRESS_WEIGHTS
#       endif
#   endif

// SC: Xenon mesh packing hack for the E3 demo
/*
#if defined(_XENON)
#define _XENON_PACK_MESH
#pragma message("ProjectConfig: _XENON_PACK_MESH - E3 HACK - Meshes will be packed and included in the shader database")
#endif
*/

#endif // defined(XENON_RENDER)

// ***********************************************************************************************************************
//    Function Prototypes
// ***********************************************************************************************************************

#if defined(_XENON)
extern void FlipEndian_32(void* Param_pBuffer, int Param_iLength);
extern void FlipEndian_16(void* Param_pBuffer, int Param_iLength);
#endif

// ***********************************************************************************************************************
//    Function hiding
// ***********************************************************************************************************************

// Make sure OutputDebugString() is disabled in entire project for final release (TCR requirement) */
#if defined(_XENON) && defined(_FINAL_)
	#ifdef OutputDebugString
		#undef OutputDebugString
	#endif
	#define OutputDebugString(x)  { /*empty*/ }
	#define OutputDebugStringA(x) { /*empty*/ }
	#define OutputDebugStringW(x) { /*empty*/ }
#endif
