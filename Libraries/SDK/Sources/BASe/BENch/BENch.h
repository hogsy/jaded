
#pragma once

#if defined __cplusplus
extern "C"
{
#endif

	// See 'MainSharedApp.cpp'
	void Jaded_Profiler_StartProfiling( const char *set );
	void Jaded_Profiler_EndProfiling( const char *set );

#if defined __cplusplus
};
#endif

#ifndef JADED_PROFILER_START
#	define JADED_PROFILER_START() Jaded_Profiler_StartProfiling( __FUNCTION__ )
#endif
#ifndef JADED_PROFILER_END
#	define JADED_PROFILER_END() Jaded_Profiler_EndProfiling( __FUNCTION__ )
#endif
