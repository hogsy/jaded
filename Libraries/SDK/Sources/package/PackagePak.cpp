// Created by Mark "hogsy" Sowden, 2023-2026 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/
// Purpose: Pak Loader, for BGE 20th Anniversary.
//			Attempts to internally convert the PAK to something we can use.

#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"
#include "FileSystem/FileSystem.h"

#include "PackagePak.h"

#include "../Extern/lz4/lib/lz4.h"
#if defined( _MSC_VER )
#	pragma comment( lib, "liblz4_static.lib" )
#endif

// Much of the below wouldn't have been possible without the work of Droolie!
// https://github.com/BinarySerializer/Ray1Map

static constexpr uint32_t PAK_MAGIC   = 'KAPB';
static constexpr uint32_t PAK_VERSION = 1;

struct PakHeader
{
	uint32_t magic;
	uint32_t version;
	uint32_t priority;
	uint32_t unk0;
	uint32_t numEntries;
	uint32_t footerSize;
};
static_assert( sizeof( PakHeader ) == 24, "invalid struct size" );

struct PakFileInfo
{
	uint32_t size;
	uint32_t compressedSize;
	uint32_t metaSize;
	uint32_t unk0;
	uint64_t offset;
};

struct PakFileTableEntry
{
	bool isKeyID;
	union
	{
		uint32_t key;
		char     name[ BIG_C_MaxLenPath ];
	} ident;

	PakFileInfo info{};
};

struct Pak
{
	PakHeader header;

	std::vector< PakFileTableEntry > files;
};

static std::vector< char > Pak_ReadFile( Pak *pak, PakFileInfo *info, FILE *file )
{
	if ( info->compressedSize == 0 && info->size == 0 )
	{
		return {};
	}

	uint64_t offset = sizeof( PakHeader ) + info->offset;
	if ( _fseeki64( file, offset, SEEK_SET ) == -1 )
	{
		char tmp[ 64 ];
		snprintf( tmp, sizeof( tmp ), "Failed to seek to file (%llu)!", offset );
		ERR_X_ForceError( tmp, nullptr );
		return {};
	}

	bool   isCompressed = info->compressedSize > 0;
	size_t readSize     = isCompressed ? info->compressedSize : info->size;

	std::vector< char > buffer;
	buffer.resize( readSize );
	fread( &buffer[ 0 ], sizeof( char ), readSize, file );

	if ( isCompressed )
	{
		std::vector< char > decompBuf;
		decompBuf.resize( info->size );
		if ( LZ4_decompress_safe( &buffer[ 0 ], &decompBuf[ 0 ], info->compressedSize, info->size ) == 0 )
		{
			char tmp[ 64 ];
			snprintf( tmp, sizeof( tmp ), "Failed to decompress file (%llu)!", offset );
			ERR_X_ForceError( tmp, nullptr );
			return {};
		}
	}

	return buffer;
}

static bool Pak_ReadHeader( Pak *pak, FILE *file )
{
	// not going to worry about endianness for now...
	// hardly think anyone is planning on getting this built on anything else right now
	fread( &pak->header, sizeof( PakHeader ), 1, file );

	if ( pak->header.magic != PAK_MAGIC )
	{
		char tmp[ 32 ];
		snprintf( tmp, sizeof( tmp ), "Invalid Pak file (%u != %u)!", pak->header.magic, PAK_MAGIC );
		ERR_X_ForceError( tmp, nullptr );
		return false;
	}

	if ( pak->header.version != PAK_VERSION )
	{
		char tmp[ 64 ];
		snprintf( tmp, sizeof( tmp ), "Unsupported Pak version (%u != %u)!", pak->header.version, PAK_VERSION );
		ERR_X_ForceError( tmp, nullptr );
		return false;
	}

	return true;
}

static bool Pak_ReadFileTable( Pak *pak, FILE *file )
{
	// need to determine size, do the ol' seeky doodle
	fseek( file, 0, SEEK_END );
	uint64_t size = _ftelli64( file );

	// seek to the end where the file table actually is
	uint64_t fileTableOffset = size - pak->header.footerSize;
	_fseeki64( file, fileTableOffset, SEEK_SET );

	for ( unsigned int i = 0; i < pak->header.numEntries; ++i )
	{
		PakFileTableEntry entry = {};

		entry.isKeyID = fgetc( file );
		if ( entry.isKeyID )
		{
			fread( &entry.ident.key, sizeof( uint32_t ), 1, file );

#if 0
			printf( "key: %u\n", entry.ident.key );
#endif
		}
		else
		{
			uint32_t nameLength;
			fread( &nameLength, sizeof( uint32_t ), 1, file );
			if ( nameLength >= sizeof( entry.ident.name ) )
			{
				char tmp[ 64 ];
				snprintf( tmp, sizeof( tmp ), "Unexpected name length in file table (%u >= %u)!", nameLength, sizeof( entry.ident.name ) );
				ERR_X_ForceError( tmp, nullptr );
				break;
			}

			fread( entry.ident.name, sizeof( char ), nameLength, file );

#if 0
			printf( "name: %s\n", entry.ident.name );
#endif
		}

		fread( &entry.info, sizeof( PakFileInfo ), 1, file );

#if 0
		printf( "meta size: %u\n"
		        "offset: %llu\n"
		        "size: %u\n"
		        "compressed size: %u\n",
		        entry.info.metaSize,
		        entry.info.offset,
		        entry.info.size,
		        entry.info.compressedSize );
#endif

		pak->files.push_back( entry );
	}

	return true;
}

bool Pak_Open( const char *path )
{
	FILE *file = fopen( path, "r+bR" );
	if ( file == nullptr )
	{
		char tmp[ 32 ];
		snprintf( tmp, sizeof( tmp ), "Failed to open Pak file!" );
		ERR_X_ForceError( tmp, nullptr );
		return false;
	}

	bool status = false;

	Pak pak = {};
	if ( !Pak_ReadHeader( &pak, file ) )
	{
		goto cleanup;
	}

	if ( !Pak_ReadFileTable( &pak, file ) )
	{
		goto cleanup;
	}

#if 0
	for ( auto &i : pak.files )
	{
		if ( i.isKeyID )
		{
			continue;
		}

		std::vector< char > buffer = Pak_ReadFile( &pak, &i.info, file );
		if ( !buffer.empty() )
		{
			if ( jaded::filesystem.CreateLocalPath( "dump" ) )
			{
				std::string name = i.isKeyID ? std::to_string( i.ident.key ) + ".bin" : i.ident.name;
				std::string path = "dump/" + name;

				FILE *out = fopen( path.c_str(), "wb" );
				if ( out != nullptr )
				{
					fwrite( &buffer[ 0 ], sizeof( char ), buffer.size(), out );
					fclose( out );
				}
			}
		}
	}
#endif

cleanup:
	fclose( file );

	return status;
}
