// File created for Jaded, the community patched Jade engine
// Purpose: Filesystem API for Jaded.
// Author:  Mark E. Sowden

#include <ShlObj.h>

#include <direct.h>

#include "fs.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
//	FileSystem API
//

std::string core::fs::FileSystem::GetExecutablePath()
{
	char filename[ 255 ];
	GetModuleFileName( nullptr, filename, sizeof( filename ) );
	return NormalizePath( filename );
}

std::string core::fs::FileSystem::GetAppDataPath( const std::string &appName )
{
	char home[ MAX_PATH ];
	if ( SUCCEEDED( SHGetFolderPath( nullptr, CSIDL_APPDATA, nullptr, 0, home ) ) )
	{
		return NormalizePath( home ) + "/" + appName;
	}

	return {};
}

std::string core::fs::FileSystem::NormalizePath( std::string path )
{
	std::replace( path.begin(), path.end(), '\\', '/' );

	size_t p;
	if ( path.size() >= 2 && std::isalpha( path[ 0 ] ) && path[ 1 ] == ':' )
	{
		p = 2;
	}
	else
	{
		p = 0;
	}

	for ( size_t i = p; i < path.size(); ++i )
	{
		static const std::string forbiddenChars = "<>:\"|?*";
		if ( forbiddenChars.find( path[ i ] ) != std::string::npos )
		{
			path[ i ] = '_';
		}
	}

	return path;
}

std::string core::fs::FileSystem::GetFilenameExtension( const std::string &filename )
{
	const size_t pos = filename.rfind( '.' );
	if ( pos == std::string::npos || pos == 0 )
	{
		return {};
	}

	std::string ext = filename.substr( pos + 1 );
	std::transform( ext.begin(), ext.end(), ext.begin(), []( char c )
	                { return ( char ) std::tolower( c ); } );
	return ext;
}

bool core::fs::FileSystem::SetWorkingDirectory( const std::string &path )
{
	if ( _chdir( path.c_str() ) != 0 )
	{
#if 0
		std::string msg = "Failed to set working directory: ";
		switch ( errno )
		{
			default:
				msg += "unknown error";
				break;
			case ENOENT:
				msg += "failed to find path";
				break;
			case EINVAL:
				msg += "invalid buffer";
				break;
		}
#endif
		//TODO:err
		return false;
	}

	return true;
}

bool core::fs::FileSystem::DoesLocalFileExist( const std::string &path )
{
	struct stat buf = {};
	return stat( path.c_str(), &buf ) == 0;
}

bool core::fs::FileSystem::CreateLocalPath( const std::string &path )
{
	// path is assumed to have been normalised prior...

	std::string dir;
	for ( const auto &i : path )
	{
		dir.push_back( i );
		if ( dir.size() > 1 && i == '/' || dir.size() == path.size() )
		{
			if ( mkdir( dir.c_str() ) == -1 && errno == ENOENT )
			{
				return false;
			}
		}
	}

	return true;
}

size_t core::fs::FileSystem::GetLocalFileSize( const std::string &path )
{
	struct stat attr = {};
	stat( path.c_str(), &attr );
	return attr.st_size;
}

time_t core::fs::FileSystem::GetLocalFileTimestamp( const std::string &path )
{
	struct stat attr = {};
	stat( path.c_str(), &attr );
	return attr.st_mtime;
}
