// Created by Mark "hogsy" Sowden, 2023-2025 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include <algorithm>

#include "Precomp.h"

#include "../MainSharedSystem.h"
#include "../Profiler.h"

#include "FileSystem.h"

#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGread.h"

#include "LINks/LINKmsg.h"

static constexpr const char *KEY_REPOSITORY = "jaded.key";

jaded::FileSystem jaded::filesystem;

std::string jaded::FileSystem::GetExecutablePath()
{
	char filename[ MAX_PATH ];
	GetModuleFileName( nullptr, filename, sizeof( filename ) );
	return NormalizePath( filename );
}

std::string jaded::FileSystem::NormalizePath( std::string path )
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

bool jaded::FileSystem::DoesFileExist( const std::string &path )
{
	struct _stat buf;
	return ( _stat( path.c_str(), &buf ) == 0 );
}

bool jaded::FileSystem::CreatePath( const std::string &path )
{
	// path is assumed to have been normalised prior...

	std::string dir;
	for ( const auto &i : path )
	{
		dir.push_back( i );
		if ( dir.size() > 1 && i == '/' || dir.size() == path.size() )
		{
#if 0
			if ( L_mkdir( dir.c_str() ) != 0 )
			{
				if ( errno == EEXIST )
				{
					continue;
				}
				return false;
			}
#else
			L_mkdir( dir.c_str() );
#endif
		}
	}

	return true;
}

void jaded::FileSystem::CreateKeyRepository( const BIG_tdst_BigFile *bf )
{
	if ( !jaded::sys::launchOperations.editorMode )
	{
		return;
	}

	//TODO: this shouldn't be an automatic operation!

	double startTime = sys::Profile::GetSeconds();

	LINK_PrintStatusMsg( "Converting Big File to key repository..." );

	std::string dstPath = GetExecutablePath();
	size_t      p       = dstPath.find_last_of( '/' );
	if ( p != std::string::npos )
	{
		dstPath.erase( p + 1 );
	}

	std::string bigPath = NormalizePath( bf->asz_Name );
	p                   = bigPath.find_last_of( '/' );

	std::string keyName = ( p != std::string::npos ) ? &bigPath[ p + 1 ] : bigPath;
	if ( ( p = keyName.find_last_of( '.' ) ) != std::string::npos )
	{
		keyName.erase( p );
	}

	std::string keyPath = dstPath + keyName + ".key";

	IndexBFSubDirectory( BIG_Root() );

	// now proceed with the creation
	FILE *file = fopen( keyPath.c_str(), "w" );
	if ( file == nullptr )
	{
		std::string msg = "Failed to create key repository per \"" + keyPath + "\"!";
		LINK_PrintStatusMsg( msg.c_str() );
		return;
	}

	fprintf( file, "%u\n", fileTable.size() );
	for ( const auto &i : fileTable )
	{
		std::string path = i.second.dir + "/" + i.second.filename;
		fprintf( file, "%x \"%s\"\n", i.first, path.c_str() );
	}
	fclose( file );

	LINK_PrintStatusMsg( "Exporting content from Big File..." );

	// now attempt to export all the files
	for ( const auto &i : fileTable )
	{
		// export it too
		std::string path = dstPath + i.second.dir;
		if ( !CreatePath( path ) )
		{
			// TODO: this should throw a more meaningful error in future
			std::string msg = "Failed to create directory (" + path + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}

		path += "/" + i.second.filename;
		if ( DoesFileExist( path ) )
		{
			continue;
		}

		file = fopen( path.c_str(), "wb" );
		if ( file == nullptr )
		{
			// TODO: this should throw a more meaningful error in future
			std::string msg = "Failed to create file (" + path + ") (" + std::to_string( i.second.key ) + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}

		ULONG size;
		char *p = BIG_pc_ReadFileTmp( BIG_PosFile( i.second.index ), &size );
		size    = BIG_fwrite( p, size, file );

		fclose( file );

		if ( size != 1 )
		{
			// TODO: this should throw a more meaningful error in future
			std::string msg = "Failed to write file data (" + path + ") (" + std::to_string( i.second.key ) + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}

		std::string msg = std::to_string( i.second.index ) + "/" + std::to_string( fileTable.size() ) + ": " + path;
		LINK_PrintStatusMsg( msg.c_str() );
	}

	double timeTaken = sys::Profile::GetSeconds() - startTime;

	std::string msg = "Took " + std::to_string( timeTaken ) + "ms to convert Big File.";
	LINK_PrintStatusMsg( msg.c_str() );
}

void jaded::FileSystem::IndexBFSubDirectory( unsigned int curDir )
{
	char dir[ BIG_C_MaxLenPath ];
	BIG_ComputeFullName( curDir, dir );

	unsigned int fileIndex = BIG_FirstFile( curDir );
	while ( fileIndex != BIG_C_InvalidIndex )
	{
		KeyFile file{};
		file.index    = fileIndex;
		file.key      = BIG_FileKey( fileIndex );
		file.filename = NormalizePath( BIG_NameFile( fileIndex ) );
		file.dir      = NormalizePath( dir );

		// HACKS!!
		//TODO: check something other than just the key here, just to be safe!!
		if ( file.key == 134265554 )
		{
			file.filename = "[0800bab3] [7200a600] OBJ_Grille_Sol_Bar.gao";
		}
		else if (file.key == 503325600)
		{
			file.filename = "1E0023A0_CopyOf_ButNotReally_Arbre_Mort_Couch_Ptite_Branche.gao";
		}

		fileTable.emplace( file.key, file );

		fileIndex = BIG_NextFile( fileIndex );
	}

	unsigned int subDir = BIG_SubDir( curDir );
	while ( subDir != BIG_C_InvalidIndex )
	{
		IndexBFSubDirectory( subDir );
		subDir = BIG_NextDir( subDir );
	}
}

extern "C" void Jaded_FileSystem_CreateKeyRepository( const BIG_tdst_BigFile *bf )
{
	jaded::filesystem.CreateKeyRepository( bf );
}
