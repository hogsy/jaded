// File created for Jaded, the community patched Jade engine
// Purpose: Filesystem API for Jaded.
// Author:  Mark E. Sowden

#include <ShlObj.h>

#include <direct.h>

#include "fs.h"

core::fs::FileSystem core::fs::filesystem = {};

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Key File
//

std::vector< uint8_t > core::fs::FileSystem::KeyFile::Read() const
{
	std::vector< uint8_t > buffer;

	FILE *file = {};

	const std::string path = GetPath();

	const size_t size = GetLocalFileSize( path );
	if ( size == ( size_t ) -1 )
	{
		throw std::runtime_error( "failed to get file size" );
	}

	buffer.resize( size );

	file = fopen( path.c_str(), "rb" );
	if ( file == nullptr )
	{
		throw std::runtime_error( "failed to open file" );
	}

	const size_t r = fread( &buffer[ 0 ], sizeof( uint8_t ), size, file );

	fclose( file );

	if ( r != size )
	{
		throw std::runtime_error( "failed to read entire file" );
	}

	return buffer;
}

std::string core::fs::FileSystem::KeyFile::GetPath() const
{
	return filesystem.directories[ dir ].name + "/" + name;
}

size_t core::fs::FileSystem::KeyFile::GetSize() const
{
	return GetLocalFileSize( GetPath() );
}

time_t core::fs::FileSystem::KeyFile::GetTimestamp() const
{
	return GetLocalFileTimestamp( GetPath() );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Key Dir
//

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

void core::fs::FileSystem::PrintKeyTable() const
{
	for ( const auto &i : files )
	{
		std::string msg = std::to_string( i.key ) + " " + directories[ i.dir ].name + "/" + i.name;
		printf( "%s\n", msg.c_str() );
	}
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

#if 0//TODO: move this out
bool core::fs::FileSystem::CreateKeyRepository( const BIG_tdst_BigFile *bf )
{
	//TODO: this shouldn't be an automatic operation!

	double startTime = sys::Profile::GetSeconds();

	LINK_PrintStatusMsg( "Converting Big File to key repository..." );

	std::string bigPath = NormalizePath( bf->asz_Name );

	size_t p = bigPath.find_last_of( '/' );

	std::string keyName = ( p != std::string::npos ) ? bigPath.substr( p + 1 ) : bigPath;
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

	if ( DoesLocalFileExist( keyPath ) )
	{
		LINK_PrintStatusMsg( "Key file already exists! Skipping extraction." );
		ParseKeyRepository( keyPath );
		return true;
	}

	if ( jaded::sys::AlertBox( "Jaded will now proceed to extract the BF and generate a " +
	                                   keyName +
	                                   ".key file. This can be a long operation.\n\n"
	                                   "Do you wish to proceed?",
	                           APP_NAME, sys::ALERT_BOX_INFO ) == sys::ALERT_BOX_RETURN_CANCEL )
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

	fprintf( file, "%x\n", universeKey );
	fprintf( file, "%zu\n", keys.size() );
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
			const std::string msg = "Failed to create directory (" + path + ")!";
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

		if ( DoesLocalFileExist( path ) )
		{
			continue;
		}

		file = fopen( path.c_str(), "wb" );
		if ( file == nullptr )
		{
			// TODO: this should throw a more meaningful error in future
			msg = "Failed to create file (" + path + ") (" + std::to_string( i.key ) + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}

		ULONG size;
		char *buf = BIG_pc_ReadFileTmp( BIG_gst.dst_FileTable[ i.bfIndex ].ul_Pos, &size );
		size      = BIG_fwrite( buf, size, file );

		fclose( file );

		if ( size != 1 )
		{
			// TODO: this should throw a more meaningful error in future
			msg = "Failed to write file data (" + path + ") (" + std::to_string( i.key ) + ")!";
			LINK_PrintStatusMsg( msg.c_str() );
			break;
		}
	}

	universeKey = BIG_gst.st_ToSave.ul_UniverseKey;

	double timeTaken = sys::Profile::GetSeconds() - startTime;

	std::string msg = "Took " + std::to_string( timeTaken ) + "ms to convert Big File.";
	LINK_PrintStatusMsg( msg.c_str() );

	return true;
}
#endif

void core::fs::FileSystem::ParseKeyRepository( const std::string &path )
{
	ClearTables();

	FILE *file = {};

	try
	{
		file = fopen( path.c_str(), "r" );
		if ( file == nullptr )
		{
			throw std::runtime_error( "failed to open key repository per \"" + path + "\"" );
		}

		//TODO: I'm being lazy here, fgets is prone to failure, I know, I'll be back!!!

		char buf[ 1024 ];

		if ( fgets( buf, sizeof( buf ), file ) == nullptr )
		{
			throw std::runtime_error( "failed to get universe key per \"" + path + "\"" );
		}

		universeKey = std::strtoul( buf, nullptr, 16 );

		if ( fgets( buf, sizeof( buf ), file ) == nullptr )
		{
			throw std::runtime_error( "failed to get number of keys per \"" + path + "\"" );
		}

		const unsigned int numKeys = std::strtoul( buf, nullptr, 10 );

		files.reserve( numKeys );

		for ( unsigned int i = 0; i < numKeys; ++i )
		{
			if ( fgets( buf, sizeof( buf ), file ) == nullptr )
			{
				throw std::runtime_error( "failed to read line per \"" + path + "\"" );
			}

			// pull the key from the start

			const char *c = strchr( buf, ' ' );
			if ( c == nullptr )
			{
				throw std::runtime_error( "failed to fetch key (" + std::string( buf ) + ")" );
			}

			char kbuf[ 16 ]{};
			strncpy( kbuf, buf, c - buf );
			Key key = strtoul( kbuf, nullptr, 16 );
			if ( key == INVALID_KEY )
			{
				throw std::runtime_error( "invalid key (" + std::string( buf ) + ")" );
			}

			// and now pull the path
			const char *openPos = strchr( buf, '"' );
			if ( openPos == nullptr )
			{
				throw std::runtime_error( "failed to find opening quote (" + std::string( buf ) + ")" );
			}
			const char *closePos = strrchr( buf, '"' );
			if ( openPos == closePos )
			{
				throw std::runtime_error( "failed to find closing quote (" + std::string( buf ) + ")" );
			}
			char pbuf[ MAX_PATH ] = {};
			strncpy( pbuf, openPos + 1, closePos - 1 - openPos );

			char *filename = strrchr( pbuf, '/' );
			if ( filename == nullptr )
			{
				throw std::runtime_error( "failed to fetch filename (" + std::string( buf ) + ")" );
			}

			KeyFile keyFile{};
			keyFile.name  = filename + 1;
			keyFile.key   = key;
			keyFile.index = files.size();

			*filename = '\0';

			const DirIndex dirIndex = IndexPath( pbuf );
			if ( dirIndex == INVALID_INDEX )
			{
				throw std::runtime_error( "failed to create directory (" + std::string( pbuf ) + ")" );
			}

			keyFile.dir = dirIndex;
			directories[ dirIndex ].files.emplace_back( keyFile.index );

			files.emplace_back( keyFile );
			fileLookup.emplace( directories[ dirIndex ].name + "/" + keyFile.name, keyFile.index );

			keys.emplace( key, keyFile.index );
		}
	}
	catch ( const std::exception & )
	{
		if ( file != nullptr )
		{
			fclose( file );
		}

		throw;
	}

	fclose( file );
}

core::fs::Key core::fs::FileSystem::GenerateFileKey( const std::string &path )
{
	Key key = 0;

#if defined( _WIN32 )
	HCRYPTPROV cProv = 0;
	HCRYPTHASH cHash = 0;

	try
	{
		if ( !CryptAcquireContext( &cProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
		{
			throw std::runtime_error( "failed to acquire crypto provider: " + std::to_string( GetLastError() ) );
		}

		if ( !CryptCreateHash( cProv, CALG_MD5, 0, 0, &cHash ) )
		{
			throw std::runtime_error( "failed to create hash: " + std::to_string( GetLastError() ) );
		}

		const BYTE *data = ( BYTE * ) path.c_str();
		if ( !CryptHashData( cHash, data, path.size(), 0 ) )
		{
			throw std::runtime_error( "failed to hash data: " + std::to_string( GetLastError() ) );
		}

		DWORD hashSize = sizeof( Key );
		if ( !CryptGetHashParam( cHash, HP_HASHVAL, ( BYTE * ) &key, &hashSize, 0 ) )
		{
			throw std::runtime_error( "failed to get hash: " + std::to_string( GetLastError() ) );
		}
	}
	catch ( const std::exception & )
	{
		if ( cProv != 0 ) CryptReleaseContext( cProv, 0 );
		if ( cHash != 0 ) CryptDestroyHash( cHash );

		throw;
	}

	if ( cProv != 0 ) CryptReleaseContext( cProv, 0 );
	if ( cHash != 0 ) CryptDestroyHash( cHash );
#endif

	return key;
}

bool core::fs::FileSystem::IsKeyTablePopulated() const
{
	return !keys.empty();
}

std::vector< core::fs::FileIndex > core::fs::FileSystem::GetDirFiles( const std::string &path )
{
	const auto i = dirLookup.find( path );
	if ( i == dirLookup.end() )
	{
		return {};
	}

	return directories[ i->second ].files;
}

core::fs::DirIndex core::fs::FileSystem::CreatePath( const std::string &path )
{
	// attempt to create the physical location first
	const std::string npath = NormalizePath( path );
	if ( !CreateLocalPath( npath ) )
	{
		return INVALID_INDEX;
	}

	return IndexPath( path );
}

core::fs::DirIndex core::fs::FileSystem::IndexPath( const std::string &path )
{
	// now work our way through
	std::string dir;
	for ( unsigned int i = 0; i < path.size(); ++i )
	{
		dir.push_back( path[ i ] );
		if ( dir.size() == path.size() || dir.size() > 1 && path[ i + 1 ] == '/' )
		{
			const auto j = dirLookup.find( dir );
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
		return INVALID_INDEX;
	}

	return j->second;
}

core::fs::FileSystem::KeyDir *core::fs::FileSystem::GetDirByName( const std::string &path )
{
	const auto &i = dirLookup.find( path );
	return i != dirLookup.end() ? &directories[ i->second ] : nullptr;
}

core::fs::FileSystem::KeyDir *core::fs::FileSystem::GetDirByIndex( const DirIndex index )
{
	if ( !filesystem.IsKeyTablePopulated() )
	{
		return nullptr;
	}

	if ( index >= directories.size() )
	{
		throw std::runtime_error( "attempted to address an OOB dir index (" + std::to_string( index ) + ")" );
	}

	return &directories[ index ];
}

core::fs::FileIndex core::fs::FileSystem::GetFileIndexByKey( Key key )
{
	const auto &i = keys.find( key );
	if ( i == keys.end() )
	{
		return INVALID_INDEX;
	}

	return i->second;
}

core::fs::FileSystem::KeyFile *core::fs::FileSystem::GetFileByKey( Key key )
{
	const auto &i = keys.find( key );
	if ( i == keys.end() )
	{
		return nullptr;
	}

	return &files[ i->second ];
}

core::fs::FileSystem::KeyFile *core::fs::FileSystem::GetFileByName( const std::string &path )
{
	const auto &i = fileLookup.find( path );
	return i != fileLookup.end() ? &files[ i->second ] : nullptr;
}

core::fs::FileSystem::KeyFile *core::fs::FileSystem::GetFileByIndex( const FileIndex index )
{
	if ( !filesystem.IsKeyTablePopulated() )
	{
		return nullptr;
	}

	if ( index >= files.size() )
	{
		throw std::runtime_error( "attempted to address an OOB file index (" + std::to_string( index ) + ")" );
	}

	return &files[ index ];
}

#if 0//TODO: move this out
void core::fs::FileSystem::IndexBFSubDirectory( const unsigned int curDir )
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
		const auto i = keys.find( file.key );
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
#endif

void core::fs::FileSystem::ClearTables()
{
	dirLookup.clear();
	directories.clear();

	fileLookup.clear();
	files.clear();

	keys.clear();
}

const core::fs::Key &core::fs::FileSystem::GetUniverseKey() const
{
	return universeKey;
}
