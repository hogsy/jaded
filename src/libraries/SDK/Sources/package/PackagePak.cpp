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

// Much of the below wouldn't have been possible without the work of Droolie!
// https://github.com/BinarySerializer/Ray1Map

std::vector< char > Pak::FileInfo::Read( FILE *file ) const
{
	if ( compressedSize == 0 && size == 0 )
	{
		return {};
	}

	const uint64_t offs = sizeof( Header ) + offset;
	if ( _fseeki64( file, offs, SEEK_SET ) == -1 )
	{
		char tmp[ 64 ];
		snprintf( tmp, sizeof( tmp ), "Failed to seek to file (%llu)!", offs );
		ERR_X_ForceError( tmp, nullptr );
		return {};
	}

	const bool   isCompressed = compressedSize > 0;
	const size_t readSize     = isCompressed ? compressedSize : size;

	std::vector< char > buffer;
	buffer.resize( readSize );
	fread( &buffer[ 0 ], sizeof( char ), readSize, file );

	if ( isCompressed )
	{
		std::vector< char > decompBuf;
		decompBuf.resize( size );
		if ( LZ4_decompress_safe( &buffer[ 0 ], &decompBuf[ 0 ], compressedSize, size ) == 0 )
		{
			char tmp[ 64 ];
			snprintf( tmp, sizeof( tmp ), "Failed to decompress file (%llu)!", offs );
			ERR_X_ForceError( tmp, nullptr );
			return {};
		}
	}

	return buffer;
}

Pak::~Pak()
{
	if ( handle != nullptr )
	{
		fclose( handle );
	}
}

bool Pak::Validate()
{
	// not going to worry about endianness for now...
	// hardly think anyone is planning on getting this built on anything else right now
	fread( &header, sizeof( Header ), 1, handle );

	if ( header.magic != MAGIC )
	{
		char tmp[ 32 ];
		snprintf( tmp, sizeof( tmp ), "Invalid Pak file (%u != %u)!", header.magic, MAGIC );
		ERR_X_ForceError( tmp, nullptr );
		return false;
	}

	if ( header.version != VERSION )
	{
		char tmp[ 64 ];
		snprintf( tmp, sizeof( tmp ), "Unsupported Pak version (%u != %u)!", header.version, VERSION );
		ERR_X_ForceError( tmp, nullptr );
		return false;
	}

	return true;
}

bool Pak::ParseTableOfContents()
{
	// need to determine size, do the ol' seeky doodle
	fseek( handle, 0, SEEK_END );
	const uint64_t size = _ftelli64( handle );

	// seek to the end where the file table actually is
	const uint64_t fileTableOffset = size - header.footerSize;
	if ( _fseeki64( handle, fileTableOffset, SEEK_SET ) != 0 )
	{
		const std::string msg = "Failed to seek to table offset (" + std::to_string( fileTableOffset ) + ")!";
		ERR_X_ForceError( msg.c_str(), nullptr );
		return false;
	}

	for ( unsigned int i = 0; i < header.numEntries; ++i )
	{
		FileTableEntry entry = {};

		entry.isKeyID = fgetc( handle );
		if ( entry.isKeyID )
		{
			fread( &entry.ident.key, sizeof( uint32_t ), 1, handle );
		}
		else
		{
			uint32_t nameLength;
			fread( &nameLength, sizeof( uint32_t ), 1, handle );
			if ( nameLength >= sizeof( entry.ident.name ) )
			{
				char tmp[ 64 ];
				snprintf( tmp, sizeof( tmp ), "Unexpected name length in file table (%u >= %u)!", nameLength, sizeof( entry.ident.name ) );
				ERR_X_ForceError( tmp, nullptr );
				return false;
			}

			fread( entry.ident.name, sizeof( char ), nameLength, handle );
		}

		fread( &entry.info, sizeof( FileInfo ), 1, handle );

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

		files.push_back( entry );

		if ( entry.isKeyID )
		{
			fileKeyLookup.emplace( entry.ident.key, files.size() - 1 );
		}
	}

	DetermineEntryPaths();

	return true;
}

Pak::FileTableEntry *Pak::GetWowForWol( const void *buf, size_t size )
{
	// so from what I can tell, we can get the primary wow as the last key from the wol...
	// this is a little dumb, but hey, we're trying to figure out what's what from a mess

	struct Index
	{
		BIG_KEY  key;
		uint32_t magic;
	};

	const Index *index = ( Index * ) ( char * ) buf + size - sizeof( Index );
	if ( index->magic != 0x776f772e )
	{
		return nullptr;
	}

	return FindEntry( index->key );
}

/**
 * Attempts to provide an appropriate extension, by figuring out the
 * type of file being dealt with.
 */
static std::string DetermineFileType( const void *buf, const size_t size )
{
	// they were nice enough to give *some* formats
	// something easy to identify with...
	if ( *( uint32_t * ) buf == 0x6f61672e )
	{
		return ".gao";
	}
	if ( *( uint32_t * ) buf == 0x61672e63 )// seems specific to 20th?
	{
		return ".cgao";
	}
	if ( *( uint32_t * ) buf == 0x776f772e )
	{
		return ".wow";
	}
	if ( *( uint32_t * ) buf == 0x494c5280 )
	{
		return ".rli";
	}

	// and now we get into cursed territory...

	if ( size >= 8 && *( ( uint32_t * ) buf + 1 ) == 0x6f61672e )
	{
		return ".gol";
	}
	if ( size >= 8 && *( ( uint32_t * ) buf + 1 ) == 0x776f772e )
	{
		return ".wol";
	}

	return ".bin";
}

void Pak::DetermineEntryPaths()
{
	for ( auto &i : files )
	{
		std::vector< char > buffer = i.info.Read( handle );
		if ( buffer.empty() )
		{
			continue;
		}

		std::string dir, name;

		std::string ident = DetermineFileType( &buffer[ 0 ], buffer.size() );
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

			// now fetch the game group key
			uint32_t groupKey;
			memcpy( &groupKey, &buffer[ 216 ], sizeof( uint32_t ) );
			if ( groupKey != BIG_C_InvalidKey && groupKey != 0 )
			{
				FileTableEntry *group = FindEntry( groupKey );
				if ( group != nullptr )
				{
					group->dstPath = dir;
					group->dstName = std::string( buf ) + ".gol";
				}
			}
		}
		else if ( ident == ".wol" )
		{
			//TODO: this shit doesn't work...
			const FileTableEntry *entry = GetWowForWol( &buffer[ 0 ], buffer.size() );
			if ( entry != nullptr )
			{
				std::vector< char > wowBuf = entry->info.Read( handle );
				assert( !wowBuf.empty() );

				std::string wowIdent = DetermineFileType( &wowBuf[ 0 ], wowBuf.size() );
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

		if ( name.empty() && i.isKeyID )
		{
			if ( i.isKeyID )
			{
				std::stringstream sstream;
				sstream << std::hex << i.ident.key;
				name = sstream.str() + ident;
			}
			else
			{
				name = i.ident.name;
			}
		}

		i.dstPath = dir;
		i.dstName = name;
	}
}

bool Pak::Open( const std::string &path )
{
	handle = fopen( path.c_str(), "r+bR" );
	if ( handle == nullptr )
	{
		char tmp[ 32 ];
		snprintf( tmp, sizeof( tmp ), "Failed to open Pak file!" );
		ERR_X_ForceError( tmp, nullptr );
		return false;
	}

	if ( !Validate() )
	{
		return false;
	}

	if ( !ParseTableOfContents() )
	{
		return false;
	}

	return true;
}

Pak::FileTableEntry *Pak::FindEntry( const uint32_t key )
{
	const auto i = fileKeyLookup.find( key );
	if ( i == fileKeyLookup.end() )
	{
		return nullptr;
	}

	return &files[ i->second ];
}

void Pak::Export( const std::string &destination ) const
{
	for ( auto &i : files )
	{
		std::vector< char > buffer = i.info.Read( handle );
		if ( buffer.empty() )
		{
			continue;
		}

		std::string path = destination + "/" + i.dstPath + "/" + i.dstName;
		if ( jaded::filesystem.DoesFileExist( path ) )
		{
			continue;
		}

		if ( !jaded::filesystem.CreateLocalPath( destination + "/" + i.dstPath ) )
		{
			char tmp[ 128 ];
			snprintf( tmp, sizeof( tmp ), "Failed to create destination (%s)!", i.dstPath.c_str() );
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
