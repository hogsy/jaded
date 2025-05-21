// Created by Mark "hogsy" Sowden, 2023-2025 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <stdlib.h>
#include <stdio.h>

#if defined( _WIN32 )
#	include <wincrypt.h>
#endif

#include "../MainSharedSystem.h"
#include "../Profiler.h"

#include "FileSystem.h"

#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGread.h"

#include "LINks/LINKmsg.h"

// sigh... we need access to the splash screen
#include "../Main/WinEditors/Sources/EDIapp.h"

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

std::string jaded::FileSystem::GetFilenameExtension( const std::string &filename )
{
	size_t pos = filename.rfind( '.' );
	if ( pos == std::string::npos || pos == 0 )
	{
		return {};
	}

	return filename.substr( pos + 1 );
}

bool jaded::FileSystem::SetWorkingDirectory( const std::string &path )
{
	if ( _chdir( path.c_str() ) != 0 )
	{
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
		LINK_PrintStatusMsg( msg.c_str() );
		return false;
	}

	return true;
}

void jaded::FileSystem::PrintKeyTable()
{
	for ( const auto &i : files )
	{
		std::string msg = std::to_string( i.key ) + " " + directories[ i.dir ].name + "/" + i.name;
		LINK_PrintStatusMsg( msg.c_str() );
	}
}

bool jaded::FileSystem::DoesFileExist( const std::string &path )
{
	struct _stat buf;
	return ( _stat( path.c_str(), &buf ) == 0 );
}

bool jaded::FileSystem::CreateLocalPath( const std::string &path )
{
	// path is assumed to have been normalised prior...

	std::string dir;
	for ( const auto &i : path )
	{
		dir.push_back( i );
		if ( dir.size() > 1 && i == '/' || dir.size() == path.size() )
		{
			L_mkdir( dir.c_str() );
		}
	}

	return true;
}

size_t jaded::FileSystem::GetLocalFileSize( const std::string &path )
{
	char dumb[ 2048 ];
	assert( _getcwd( dumb, sizeof( dumb ) ) != nullptr );

	FILE *file = fopen( path.c_str(), "rb" );
	if ( file == nullptr )
	{
		return ( size_t ) -1;
	}

	struct stat buf;
	int         fd = fileno( file );
	fstat( fd, &buf );

	fclose( file );

	return buf.st_size;
}

bool jaded::FileSystem::ReadFileByIndex( FileIndex index, std::vector< uint8_t > *dst )
{
	bool  status;
	FILE *file{};
	try
	{
		std::string path = GetFilePathByIndex( index );
		if ( path.empty() )
		{
			throw std::runtime_error( "failed to get filename" );
		}

		size_t size = GetLocalFileSize( path );
		if ( size == ( size_t ) -1 )
		{
			throw std::runtime_error( "failed to get file size" );
		}

		dst->resize( size );

		file = fopen( path.c_str(), "rb" );
		if ( file == nullptr )
		{
			throw std::runtime_error( "failed to open file" );
		}

		if ( fread( dst->data(), sizeof( uint8_t ), size, file ) != size )
		{
			throw std::runtime_error( "failed to read entire file" );
		}

		status = true;
	}
	catch ( const std::exception &e )
	{
		std::string msg = "Failed to read file: " + std::string( e.what() );
		LINK_PrintStatusMsg( e.what() );
		status = false;
	}

	if ( file != nullptr )
	{
		fclose( file );
	}

	return status;
}

bool jaded::FileSystem::ReadFileByName( const std::string &path, std::vector< uint8_t > *dst )
{
	auto &i = fileLookup.find( path );
	if ( i == fileLookup.end() )
	{
		return false;
	}

	return ReadFileByIndex( i->second, dst );
}

bool jaded::FileSystem::SetProject( const std::string &path )
{
	std::string npath = NormalizePath( path );

	std::string wd = npath;
	size_t      p  = wd.find_last_of( '/' );
	if ( p != std::string::npos )
	{
		wd.erase( p );
	}

	SetWorkingDirectory( wd );

	std::string extension = GetFilenameExtension( npath );
	if ( extension == "bf" || extension == "BF" )
	{
		if ( !BIG_Open( path.c_str() ) )
		{
			std::string msg = "Failed to open big file (" + npath + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			return false;
		}

		if ( !CreateKeyRepository( &BIG_gst ) )
		{
			std::string msg = "Failed to create key repository (" + npath + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			return false;
		}
	}
	else if ( !ParseKeyRepository( npath ) )
	{
		std::string msg = "Failed to open key file (" + npath + ")!";
		LINK_PrintStatusMsg( msg.c_str() );
		return false;
	}

	return true;
}

bool jaded::FileSystem::CreateKeyRepository( const BIG_tdst_BigFile *bf )
{
	if ( !jaded::sys::launchOperations.editorMode )
	{
		return false;
	}

	//TODO: this shouldn't be an automatic operation!

	double startTime = sys::Profile::GetSeconds();

	LINK_PrintStatusMsg( "Converting Big File to key repository..." );


	std::string bigPath = NormalizePath( bf->asz_Name );
	size_t      p;
	p = bigPath.find_last_of( '/' );

	std::string keyName = ( p != std::string::npos ) ? &bigPath[ p + 1 ] : bigPath;
	if ( ( p = keyName.find_last_of( '.' ) ) != std::string::npos )
	{
		keyName.erase( p );
	}

	std::string dstPath = bigPath;
	p                   = dstPath.find_last_of( '/' );
	if ( p != std::string::npos )
	{
		dstPath.erase( p + 1 );
	}

	std::string keyPath = dstPath + keyName + ".key";

	if ( DoesFileExist( keyPath ) )
	{
		LINK_PrintStatusMsg( "Key file already exists! Skipping extraction." );
		ParseKeyRepository( keyPath );
		return true;
	}

	if ( jaded::sys::AlertBox( "Jaded will now proceed to extract the BF and generate a " +
	                                   keyName +
	                                   ".key file. This can be a long operation.\n\n"
	                                   "Do you wish to proceed?",
	                           "Jaded", jaded::sys::ALERT_BOX_INFO ) == jaded::sys::ALERT_BOX_RETURN_CANCEL )
	{
		return false;
	}

	ClearTables();

	IndexBFSubDirectory( BIG_Root() );

	// now proceed with the creation
	FILE *file = fopen( keyPath.c_str(), "w" );
	if ( file == nullptr )
	{
		std::string msg = "Failed to create key repository per \"" + keyPath + "\"!";
		LINK_PrintStatusMsg( msg.c_str() );
		return false;
	}

	fprintf( file, "%u\n", keys.size() );
	for ( const auto &i : keys )
	{
		const KeyFile *fPtr = &files[ i.second ];
		const KeyDir  *dPtr = &directories[ fPtr->dir ];

		std::string path = dPtr->name + "/" + fPtr->name;
		fprintf( file, "%x \"%s\"\n", i.first, path.c_str() );
	}
	fclose( file );

	LINK_PrintStatusMsg( "Exporting content from Big File..." );

	// now attempt to export all the files
	for ( const auto &i : files )
	{
		// export it too
		std::string path = dstPath + directories[ i.dir ].name;
		if ( !CreateLocalPath( path ) )
		{
			// TODO: this should throw a more meaningful error in future
			std::string msg = "Failed to create directory (" + path + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}

		path += "/" + i.name;

		// spit out the information so the user knows something is happening
		std::string msg = std::to_string( i.index ) + "/" + std::to_string( keys.size() ) + ": " + path;
		LINK_PrintStatusMsg( msg.c_str() );
		if ( EDI_gpo_EnterWnd != nullptr )
		{
			// use a shorter name for the UI just to make it easier to see
			msg = std::to_string( i.index ) + "/" + std::to_string( keys.size() ) + ": " + directories[ i.dir ].name + "/" + i.name;
			EDI_gpo_EnterWnd->DisplayMessage( msg.c_str() );
		}

		if ( DoesFileExist( path ) )
		{
			continue;
		}

		file = fopen( path.c_str(), "wb" );
		if ( file == nullptr )
		{
			// TODO: this should throw a more meaningful error in future
			std::string msg = "Failed to create file (" + path + ") (" + std::to_string( i.key ) + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}

		ULONG size;
		char *p = BIG_pc_ReadFileTmp( BIG_PosFile( i.bfIndex ), &size );
		size    = BIG_fwrite( p, size, file );

		fclose( file );

		if ( size != 1 )
		{
			// TODO: this should throw a more meaningful error in future
			std::string msg = "Failed to write file data (" + path + ") (" + std::to_string( i.key ) + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}
	}

	double timeTaken = sys::Profile::GetSeconds() - startTime;

	std::string msg = "Took " + std::to_string( timeTaken ) + "ms to convert Big File.";
	LINK_PrintStatusMsg( msg.c_str() );

	return true;
}

bool jaded::FileSystem::ParseKeyRepository( const std::string &path )
{
	ClearTables();

	bool  status{};
	FILE *file{};

	try
	{
		file = fopen( path.c_str(), "r" );
		if ( file == nullptr )
		{
			throw std::runtime_error( "Failed to open key repository per \"" + path + "\"!" );
		}

		//TODO: I'm being lazy here, fgets is prone to failure, I know, I'll be back!!!

		char buf[ 1024 ];
		if ( fgets( buf, sizeof( buf ), file ) == nullptr )
		{
			throw std::runtime_error( "Failed to get number of keys per \"" + path + "\"!" );
		}

		unsigned int numKeys = std::strtoul( buf, nullptr, 10 );
		for ( unsigned int i = 0; i < numKeys; ++i )
		{
			if ( fgets( buf, sizeof( buf ), file ) == nullptr )
			{
				throw std::runtime_error( "Failed to read line per \"" + path + "\"!" );
			}

			// pull the key from the start

			char *c = strchr( buf, ' ' );
			if ( c == nullptr )
			{
				throw std::runtime_error( "Failed to fetch key (" + std::string( buf ) + ")!" );
			}

			char kbuf[ 16 ]{};
			strncpy( kbuf, buf, c - buf );
			Key key = strtoul( kbuf, nullptr, 16 );
			if ( key == 0 )
			{
				throw std::runtime_error( "Invalid key (" + std::string( buf ) + ")!" );
			}

			// and now pull the path
			char *openPos = strchr( buf, '"' );
			if ( openPos == nullptr )
			{
				throw std::runtime_error( "Failed to find opening quote (" + std::string( buf ) + ")!" );
			}
			char *closePos = strrchr( buf, '"' );
			if ( openPos == closePos )
			{
				throw std::runtime_error( "Failed to find closing quote (" + std::string( buf ) + ")!" );
			}
			char pbuf[ L_MAX_PATH ]{};
			strncpy( pbuf, openPos + 1, ( closePos - 1 ) - openPos );

			char *filename = strrchr( pbuf, '/' );
			if ( filename == nullptr )
			{
				throw std::runtime_error( "Failed to fetch filename (" + std::string( buf ) + ")!" );
			}

			KeyFile keyFile{};
			keyFile.name  = filename + 1;
			keyFile.key   = key;
			keyFile.index = files.size();

			*filename = '\0';

			DirIndex dirIndex = IndexPath( pbuf );
			if ( dirIndex == BIG_C_InvalidIndex )
			{
				throw std::runtime_error( "Failed to create directory (" + std::string( pbuf ) + ")!" );
			}

			keyFile.dir = dirIndex;
			directories[ dirIndex ].files.emplace_back( keyFile.index );

			files.emplace_back( keyFile );
			fileLookup.emplace( directories[ dirIndex ].name + "/" + keyFile.name, keyFile.index );
		}

		status = true;
	}
	catch ( const std::exception &e )
	{
		LINK_PrintStatusMsg( e.what() );
	}

	if ( file != nullptr )
	{
		fclose( file );
	}

	return status;
}

jaded::FileSystem::Key jaded::FileSystem::GenerateFileKey( const std::string &path )
{
	Key key = 0;

#if defined( _WIN32 )
	HCRYPTPROV cProv = 0;
	HCRYPTHASH cHash = 0;
	try
	{
		if ( !CryptAcquireContext( &cProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
		{
			throw std::runtime_error( "Failed to acquire crypto provider: " + std::to_string( GetLastError() ) );
		}

		if ( !CryptCreateHash( cProv, CALG_MD5, 0, 0, &cHash ) )
		{
			throw std::runtime_error( "Failed to create hash: " + std::to_string( GetLastError() ) );
		}

		const BYTE *data = ( BYTE * ) path.c_str();
		if ( !CryptHashData( cHash, data, path.size(), 0 ) )
		{
			throw std::runtime_error( "Failed to hash data: " + std::to_string( GetLastError() ) );
		}

		DWORD hashSize = sizeof( Key );
		if ( !CryptGetHashParam( cHash, HP_HASHVAL, ( BYTE * ) &key, &hashSize, 0 ) )
		{
			throw std::runtime_error( "Failed to get hash: " + std::to_string( GetLastError() ) );
		}
	}
	catch ( const std::exception &e )
	{
		LINK_PrintStatusMsg( e.what() );
	}

	if ( cProv != 0 ) CryptReleaseContext( cProv, 0 );
	if ( cHash != 0 ) CryptDestroyHash( cHash );
#endif

	return key;
}

const std::vector< jaded::FileSystem::FileIndex > &jaded::FileSystem::GetDirFiles( const std::string &path )
{
	auto &i = dirLookup.find( path );
	if ( i == dirLookup.end() )
	{
		std::string msg = "Failed to find directory (" + path + ")!";
		LINK_PrintStatusMsg( msg.c_str() );
		return {};
	}

	return directories[ i->second ].files;
}

jaded::FileSystem::DirIndex jaded::FileSystem::CreatePath( const std::string &path )
{
	// attempt to create the physical location first
	std::string npath = NormalizePath( path );
	if ( !CreateLocalPath( npath ) )
	{
		return BIG_C_InvalidIndex;
	}

	return IndexPath( path );
}

jaded::FileSystem::DirIndex jaded::FileSystem::IndexPath( const std::string &path )
{
	// now work our way through
	std::string dir;
	for ( unsigned int i = 0; i < path.size(); ++i )
	{
		dir.push_back( path[ i ] );
		if ( dir.size() == path.size() || dir.size() > 1 && path[ i + 1 ] == '/' )
		{
			auto &j = dirLookup.find( dir );
			if ( j != dirLookup.end() )
			{
				continue;
			}

			KeyDir directory{};
			directory.index = directories.size();
			directory.name  = dir;

			directories.emplace_back( directory );
			dirLookup.emplace( directory.name, directory.index );
		}
	}

	const auto &j = dirLookup.find( dir );
	if ( j == dirLookup.end() )
	{
		return BIG_C_InvalidIndex;
	}

	return j->second;
}

jaded::FileSystem::KeyDir *jaded::FileSystem::GetDirByName( const std::string &path )
{
	const auto &i = dirLookup.find( path );
	return i != dirLookup.end() ? &directories[ i->second ] : nullptr;
}

jaded::FileSystem::KeyFile *jaded::FileSystem::GetFileByName( const std::string &path )
{
	const auto &i = fileLookup.find( path );
	return i != fileLookup.end() ? &files[ i->second ] : nullptr;
}

jaded::FileSystem::KeyFile *jaded::FileSystem::GetFileByIndex( FileIndex index )
{
	if ( index >= files.size() )
	{
		std::string msg = "Attempted to address an OOB file index (" + std::to_string( index ) + ")!";
		LINK_PrintStatusMsg( msg.c_str() );
		return nullptr;
	}

	return &files[ index ];
}

std::string jaded::FileSystem::GetFilePathByIndex( FileIndex index )
{
	return directories[ files[ index ].dir ].name + "/" + files[ index ].name;
}

void jaded::FileSystem::IndexBFSubDirectory( unsigned int curDir )
{
	char dir[ BIG_C_MaxLenPath ];
	BIG_ComputeFullName( curDir, dir );

	KeyDir directory{};
	directory.name  = dir;
	directory.index = directories.size();
	directories.emplace_back( directory );

	// ensure that it's in our lookup table
	dirLookup.emplace( directory.name, directory.index );

	FileIndex fileIndex = BIG_FirstFile( curDir );
	while ( fileIndex != BIG_C_InvalidIndex )
	{
		KeyFile file{};
		file.index   = files.size();
		file.bfIndex = fileIndex;
		file.key     = BIG_FileKey( fileIndex );
		file.name    = NormalizePath( BIG_NameFile( fileIndex ) );
		file.dir     = directory.index;

		// HACKS!!
		//TODO: check something other than just the key here, just to be safe!!
		if ( file.key == 134265554 )
		{
			file.name = "[0800bab3] [7200a600] OBJ_Grille_Sol_Bar.gao";
		}
		else if ( file.key == 503325600 )
		{
			file.name = "1E0023A0_CopyOf_ButNotReally_Arbre_Mort_Couch_Ptite_Branche.gao";
		}

		files.emplace_back( file );

		// add the file under the directory files listing too
		KeyDir *dirPtr = &directories.back();
		dirPtr->files.emplace_back( file.index );

		fileLookup.emplace( directory.name + "/" + file.name, file.index );

		// check if it's a duplicate key first (it could happen...)
		auto &i = keys.find( file.key );
		if ( i != keys.end() )
		{
			std::string msg = "Found duplicate key (" + std::to_string( i->first ) + ")";
			LINK_PrintStatusMsg( msg.c_str() );
		}

		keys.emplace( file.key, file.index );

		fileIndex = BIG_NextFile( fileIndex );
	}

	DirIndex subDir = BIG_SubDir( curDir );
	while ( subDir != BIG_C_InvalidIndex )
	{
		IndexBFSubDirectory( subDir );
		subDir = BIG_NextDir( subDir );
	}
}

void jaded::FileSystem::ClearTables()
{
	dirLookup.clear();
	directories.clear();

	fileLookup.clear();
	files.clear();

	keys.clear();
}

// C interface for legacy BIG API

extern "C" uint32_t Jaded_FileSystem_GenerateFileKey( const char *path )
{
	return jaded::filesystem.GenerateFileKey( path );
}

extern "C" uint32_t Jaded_FileSystem_SearchFileExt( const char *path )
{
	jaded::FileSystem::KeyFile *file = jaded::filesystem.GetFileByName( path );
	return file != nullptr ? file->index : BIG_C_InvalidIndex;
}

extern "C" uint32_t Jaded_FileSystem_CreatePath( const char *path )
{
	return jaded::filesystem.CreatePath( path );
}

extern "C" uint32_t Jaded_FileSystem_LookupDirectory( const char *path )
{
	jaded::FileSystem::KeyDir *dir = jaded::filesystem.GetDirByName( path );
	return dir != nullptr ? dir->index : BIG_C_InvalidIndex;
}
