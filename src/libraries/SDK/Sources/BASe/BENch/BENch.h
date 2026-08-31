
#pragma once

#if defined __cplusplus
extern "C"
{
#endif

	// See 'MainSharedApp.cpp'
	void Jaded_Profiler_StartProfiling( unsigned int set );
	void Jaded_Profiler_EndProfiling( unsigned int set );

	void Jaded_Profiler_StartProfilingS( const char *name );
	void Jaded_Profiler_EndProfilingS( const char *name );

#if defined __cplusplus
};
#endif

#define BEN_FUNCTION_START() Jaded_Profiler_StartProfilingS( __FUNCTION__ )
#define BEN_FUNCTION_END()   Jaded_Profiler_EndProfilingS( __FUNCTION__ )

#define _GSP_BeginRaster( az ) Jaded_Profiler_StartProfiling( ( az ) )
#define _GSP_EndRaster( az )   Jaded_Profiler_EndProfiling( ( az ) );
