// ************************************************
// Simplified Thread Event Manager
//
// Used for King Kong Xenon Multi-Core Optimization
// By Alexandre David (February 2005)
// ************************************************

#ifndef __EventThread_Native_h__
#define __EventThread_Native_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "BASe/MEMory/MEM.h"

// ***********************************************************************************************************************

namespace EventThread
{

// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	eThreadID_Default,
	eThreadID_RenderingFlip,
	eThreadID_SoundStreams,
	eThreadID_DebugUpdates,

	eThreadID_Count
}
eThreadID;

// ***********************************************************************************************************************
//    Structs
// ***********************************************************************************************************************

typedef struct
{
	char*	pName;
	int		iProcessorCoreID;
	int		iPriority;
}
stThreadConfiguration;

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

const int kiProcessor_Core_0_Thread_0 = 0;
const int kiProcessor_Core_0_Thread_1 = 1;
const int kiProcessor_Core_1_Thread_0 = 2;
const int kiProcessor_Core_1_Thread_1 = 3;
const int kiProcessor_Core_2_Thread_0 = 4;
const int kiProcessor_Core_2_Thread_1 = 5;

// ***********************************************************************************************************************

const stThreadConfiguration kastThreadConfiguration[eThreadID_Count] =
{
	// Default

	{
		"KK - Default",
		kiProcessor_Core_0_Thread_1,
		THREAD_PRIORITY_NORMAL
	},

	// Rendering Flip

	{
		"KK - Rendering Flip",
		kiProcessor_Core_1_Thread_0,
		THREAD_PRIORITY_NORMAL
	},

	// Sound Streams

	{
		"KK - Sound Streams",
		kiProcessor_Core_2_Thread_0,
		THREAD_PRIORITY_NORMAL
	},

	// Debug Updates (Currently Unused)

	{
		"KK - Debug Updates",
		kiProcessor_Core_2_Thread_1,
		THREAD_PRIORITY_LOWEST
	}
};

// ***********************************************************************************************************************

} // Namespace

// ***********************************************************************************************************************

#endif // __EventThread_Native_h__
