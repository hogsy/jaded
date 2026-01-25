// File created for Jaded, the community patched Jade engine

#pragma once

#include <string>

//TODO: this should go somewhere else...
#define JADED_STRINGIFY( num ) #num
#define JADED_TOSTRING( A )    JADED_STRINGIFY( A )

#define JADED_VERSION_MAJOR 0
#define JADED_VERSION_MINOR 4
#define JADED_VERSION_PATCH 0

namespace jaded
{
	//TODO: this should go somewhere else...
	static constexpr char APP_NAME[] = "Jaded";

	static constexpr char VERSION_STRING[] = "v" JADED_TOSTRING( JADED_VERSION_MAJOR ) "." JADED_TOSTRING( JADED_VERSION_MINOR ) "." JADED_TOSTRING( JADED_VERSION_PATCH );
#if !defined( NDEBUG )
	static constexpr char BUILD_STRING[]   = "Debug (" GIT_COMMIT_HASH ")";
#else
	static constexpr char BUILD_STRING[]   = "Release (" GIT_COMMIT_HASH ")";
#endif

	static const std::string &GetReleaseVersion()
	{
		static const std::string version = VERSION_STRING + std::string( " " ) + BUILD_STRING;
		return version;
	}
}// namespace jaded
