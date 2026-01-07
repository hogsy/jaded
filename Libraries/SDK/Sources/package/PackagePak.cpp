// Created by Mark "hogsy" Sowden, 2023-2026 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/
// Purpose: Pak Loader, for BGE 20th Anniversary.
//			Attempts to internally convert the PAK to something we can use.

#include <sstream>
#include <ios>

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

	std::vector< PakFileTableEntry >   files;
	std::map< uint32_t, unsigned int > fileKeyLookup;
};

static std::vector< char > Pak_ReadFile( Pak *pak, const PakFileInfo *info, FILE *file )
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

		if ( entry.isKeyID )
		{
			pak->fileKeyLookup.emplace( entry.ident.key, pak->files.size() - 1 );
		}
	}

	return true;
}

/**
 * Attempts to provide an appropriate extension, by figuring out the
 * type of file being dealt with.
 */
static std::string Pak_DetermineFileType( const void *buf, size_t size )
{
	// they were nice enough to give *some* formats
	// something easy to identify with...
	if ( *( ( uint32_t * ) buf ) == 0x6f61672e )
	{
		return ".gao";
	}
	if ( *( ( uint32_t * ) buf ) == 0x61672e63 )// seems specific to 20th?
	{
		return ".cgao";
	}
	if ( *( ( uint32_t * ) buf ) == 0x776f772e )
	{
		return ".wow";
	}
	if ( *( ( uint32_t * ) buf ) == 0x494c5280 )
	{
		return ".rli";
	}

	// and now we get into cursed territory...

	if ( size >= 8 && *( ( ( uint32_t * ) buf ) + 1 ) == 0x6f61672e )
	{
		return ".gol";
	}
	if ( size >= 8 && *( ( ( uint32_t * ) buf ) + 1 ) == 0x776f772e )
	{
		return ".wol";
	}

	return ".bin";
}

// Hacky gross crap ...

static const PakFileTableEntry *GetWowForWol( const Pak *pak, const void *buf, size_t size )
{
	// so from what I can tell, we can get the primary wow as the last key from the wol...
	// this is a little dumb, but hey, we're trying to figure out what's what from a mess

	struct Index
	{
		BIG_KEY  key;
		uint32_t magic;
	};

	Index *index = ( Index * ) ( ( char * ) buf ) + size - sizeof( Index );
	if ( index->magic != 0x776f772e )
	{
		return nullptr;
	}

	auto &i = pak->fileKeyLookup.find( index->key );
	if ( i == pak->fileKeyLookup.end() )
	{
		return nullptr;
	}

	return &pak->files[ i->second ];
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

	for ( auto &i : pak.files )
	{
		std::vector< char > buffer = Pak_ReadFile( &pak, &i.info, file );
		if ( !buffer.empty() )
		{
			std::string dir, name;
			if ( i.isKeyID )
			{
				std::string ident = Pak_DetermineFileType( &buffer[ 0 ], buffer.size() );
				if ( ident == ".wow" )
				{
					// we can pull the original filename from a wow :)
					// this should be safe; size is technically 60,
					// but last four bytes are unused and string is
					// null-terminated
					char buf[ 64 ] = {};
					memcpy( buf, &buffer[ 16 ], 60 );

					dir  = "ROOT/06 Levels/" + std::string( buf );
					name = std::string( buf ) + ".wow";
				}
				else if ( ident == ".wol" )
				{
					//TODO: this shit doesn't work...
					const PakFileTableEntry *entry = GetWowForWol( &pak, &buffer[ 0 ], buffer.size() );
					if ( entry != nullptr )
					{
						std::vector< char > wowBuf = Pak_ReadFile( &pak, &entry->info, file );
						assert( !wowBuf.empty() );

						std::string wowIdent = Pak_DetermineFileType( &wowBuf[ 0 ], wowBuf.size() );
						assert( wowIdent == ".wow" );

						char buf[ 64 ] = {};
						memcpy( buf, &wowBuf[ 16 ], 60 );

						dir  = "ROOT/06 Levels/" + std::string( buf );
						name = std::string( buf ) + ".wol";
					}
				}

				if ( dir.empty() )
				{
					dir = "ROOT/Unsorted";
				}

				if ( name.empty() )
				{
					std::stringstream sstream;
					sstream << std::hex << i.ident.key;
					name = sstream.str() + ident;
				}
			}
			else
			{
				name = i.ident.name;
				dir  = "ROOT/Unsorted";
			}

			std::string path = dir + "/" + name;
			if ( jaded::filesystem.DoesFileExist( path ) )
			{
				continue;
			}

			if ( !jaded::filesystem.CreateLocalPath( dir ) )
			{
				char tmp[ 32 ];
				snprintf( tmp, sizeof( tmp ), "Failed to create destination (%s)!", dir.c_str() );
				ERR_X_ForceError( tmp, nullptr );
				continue;
			}

			FILE *out = fopen( path.c_str(), "wb" );
			if ( out != nullptr )
			{
				fwrite( &buffer[ 0 ], sizeof( char ), buffer.size(), out );
				fclose( out );
			}
		}
	}

cleanup:
	fclose( file );

	return status;
}
