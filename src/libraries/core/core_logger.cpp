// File created for Jaded, the community patched Jade engine
// Purpose: Unified logging API
// Author:  Mark E. Sowden

#include <cstdarg>
#include <ctime>

#include "core.h"
#include "core_logger.h"

void core::Logger::WriteMessage( FILE *file, const std::string &msg, const time_t timestamp )
{
	char buf[ 64 ];
	strftime( buf, sizeof( buf ), "%x %I:%M%p", localtime( &timestamp ) );
	fprintf( file, "[%s] %s", buf, msg.c_str() );
}

void core::Logger::PushMessage( const Channel::Type type, const char *msg, ... )
{
	va_list args;
	va_start( args, msg );

	va_list argsc;
	va_copy( argsc, args );

	const int length = vsnprintf( nullptr, 0, msg, args );
	va_end( args );

	if ( length > 0 )
	{
		std::vector< char > arr( length + 1 );
		vsnprintf( arr.data(), arr.size(), msg, argsc );
		messages.emplace_back( arr.data(), time( nullptr ), type );

		for ( const auto &callback : callbacks )
		{
			callback( type, arr.data() );
		}

		if ( out != nullptr )
		{
			const Message &message = messages.back();
			WriteMessage( out, message.string, message.timestamp );
		}

		if ( messages.size() >= MAX_MESSAGES )
		{
			messages.erase( messages.begin() );
		}
	}

	va_end( argsc );
}

void core::Logger::AddCallback( const Channel::Type type, const Callback &callback )
{
	callbacks.push_back( callback );
}

bool core::Logger::SetOutputFile( const std::string &path )
{
	assert( out == nullptr );

	out = fopen( path.c_str(), "w" );
	if ( out == nullptr )
	{
		return false;
	}

	return true;
}

void core::Logger::DumpOutput( const std::string &path ) const
{
	FILE *file = fopen( path.c_str(), "w" );
	if ( file == nullptr )
	{
		return;
	}

	for ( const auto &i : messages )
	{
		WriteMessage( file, i.string, i.timestamp );
	}

	fclose( file );
}
