// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/
#pragma once

#include <map>
#include <string>
#include <cassert>
#include <ctime>

#if defined( _MSC_VER )
#	include <Windows.h>
#endif

namespace jaded
{
	namespace sys
	{
		class Profile
		{
		private:
			inline static bool GetTime( struct timespec *ts )
			{
#if defined( _MSC_VER )
				/* based on public-domain implementation here;
				 * https://github.com/msys2-contrib/mingw-w64/blob/master/mingw-w64-libraries/winpthreads/src/clock.c
				 */
				LARGE_INTEGER pf;
				if ( QueryPerformanceFrequency( &pf ) == 0 )
				{
					return false;
				}

				LARGE_INTEGER pc;
				if ( QueryPerformanceCounter( &pc ) == 0 )
				{
					return false;
				}

				static constexpr LONGLONG POW10_9 = 1000000000;

				ts->tv_sec  = pc.QuadPart / pf.QuadPart;
				ts->tv_nsec = ( int ) ( ( ( pc.QuadPart % pf.QuadPart ) * POW10_9 + ( pf.QuadPart >> 1 ) ) / pf.QuadPart );
				if ( ts->tv_nsec >= POW10_9 )
				{
					ts->tv_sec++;
					ts->tv_nsec -= POW10_9;
				}
#else
				if ( clock_gettime( CLOCK_MONOTONIC, ts ) != 0 )
				{
					return false;
				}
#endif

				return true;
			}

			/** \fn double timespec_to_double(struct timespec ts)
			 *  \brief Converts a timespec to a fractional number of seconds.
			 *  Originally written by Daniel Collins (2017), used with permission.
			 */
			inline static double TimespecToDouble( const struct timespec *ts )
			{
				static constexpr unsigned int NSEC_PER_SEC = 1000000000;
				return ( ( double ) ( ts->tv_sec ) + ( ( double ) ( ts->tv_nsec ) / NSEC_PER_SEC ) );
			}

		public:
			inline static double GetSeconds()
			{
				struct timespec ts;
				if ( !GetTime( &ts ) )
				{
					return 0;
				}

				return TimespecToDouble( &ts );
			}

		private:
			double timeStarted{};// time current frame started
			double timeTaken{};  // time current frame took

			float colour[ 3 ]{};// colours for ui

		public:
			inline Profile()
			{
				// setup colour that we'll display in the profiler UI
				for ( uint8_t i = 0; i < 3; ++i )
				{
					colour[ i ] = ( ( rand() % 200 ) + 50 ) / 255;
				}

				Start();
			}

			inline void Start()
			{
				timeStarted = GetSeconds() * 1000.0;
			}

			inline void End()
			{
				timeTaken = ( GetSeconds() * 1000.0 ) - timeStarted;
			}

			inline double GetTimeTaken() const
			{
				return timeTaken;
			}
		};

		class Profiler
		{
		private:
			bool isActive{};

		public:
			Profiler()  = default;
			~Profiler() = default;

			inline void SetActive( bool state )
			{
				isActive = state;
			}

			inline bool GetState() const
			{
				return isActive;
			}

			void   StartProfiling( const std::string &set );
			double EndProfiling( const std::string &set );

			typedef std::map< std::string, Profile > ProfileMap;

		private:
			ProfileMap profSets;

		public:
			const ProfileMap &GetProfilerSets()
			{
				return profSets;
			}
		};

		extern Profiler profiler;
	}// namespace sys
}// namespace jaded

#define JADED_PROFILER_START() jaded::sys::profiler.StartProfiling( __FUNCTION__ )
#define JADED_PROFILER_END()   jaded::sys::profiler.EndProfiling( __FUNCTION__ )
#define JADED_PROFILER_CALL( X )                   \
	{                                              \
		jaded::sys::profiler.StartProfiling( #X ); \
		X;                                         \
		jaded::sys::profiler.EndProfiling( #X );   \
	}
