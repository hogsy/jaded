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
			inline bool GetTime( struct timespec *ts )
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

				static constexpr LONGLONG POW10_7 = 10000000;
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
			inline double TimespecToDouble( const struct timespec *ts )
			{
				static constexpr unsigned int NSEC_PER_SEC = 1000000000;
				return ( ( double ) ( ts->tv_sec ) + ( ( double ) ( ts->tv_nsec ) / NSEC_PER_SEC ) );
			}

			inline double GetSeconds()
			{
				struct timespec ts;
				if ( !GetTime( &ts ) )
				{
					return 0;
				}

				return TimespecToDouble( &ts );
			}

			double timeStarted{};
			double timeTaken{};
			double oldTimeTaken{};

		public:
			inline Profile()
			{
				Start();
			}

			inline void Start()
			{
				timeStarted = GetSeconds() * 1000.0;
			}

			inline void End()
			{
				oldTimeTaken = timeTaken;
				timeTaken    = ( GetSeconds() * 1000.0 ) - timeStarted;
			}

			inline double GetTimeTaken() const
			{
				return timeTaken;
			}
		};

		class Profiler
		{
		public:
			inline void StartProfiling( const std::string &set )
			{
				auto i = profSets.find( set );
				if ( i == profSets.end() )
				{
					profSets.emplace( set, Profile() );
					return;
				}

				i->second.Start();
			}

			inline void EndProfiling( const std::string &set )
			{
				auto i = profSets.find( set );
				assert( i != profSets.end() );
				i->second.End();
			}

		private:
			std::map< std::string, Profile > profSets;

		public:
			const std::map< std::string, Profile > &GetProfilerSets()
			{
				return profSets;
			}
		};

		extern Profiler profiler;
	}// namespace sys
}// namespace jaded

#define JADED_PROFILER_START() jaded::sys::profiler.StartProfiling( __FUNCTION__ )
#define JADED_PROFILER_END()   jaded::sys::profiler.EndProfiling( __FUNCTION__ )
