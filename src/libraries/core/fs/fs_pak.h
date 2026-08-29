// File created for Jaded, the community patched Jade engine

#pragma once

#include "../core.h"

namespace core::fs
{
	class Pak
	{
		static constexpr uint32_t MAGIC   = 'KAPB';
		static constexpr uint32_t VERSION = 1;

		struct Header
		{
			uint32_t magic;
			uint32_t version;
			uint32_t priority;
			uint32_t unk0;
			uint32_t numEntries;
			uint32_t footerSize;
		};
		static_assert( sizeof( Header ) == 24, "invalid struct size" );

	public:
		struct FileInfo
		{
			uint32_t size;
			uint32_t compressedSize;
			uint32_t metaSize;
			uint64_t offset;

			std::vector< char > Read( FILE *file ) const;
		};
		static_assert( sizeof( FileInfo ) == 24, "invalid struct size" );

		struct FileTableEntry
		{
			bool isKeyID;

			uint32_t key;

			std::string path;
			std::string name;

			FileInfo info{};
		};

	private:
		Header header{};

		std::vector< FileTableEntry >      files;
		std::map< uint32_t, unsigned int > fileKeyLookup;

		FILE *handle;

	public:
		Pak() = default;
		~Pak();

	private:
		bool Validate();
		bool ParseTableOfContents();

		FileTableEntry *GetWowForWol( const void *buf, size_t size );

		void ProcessWow( FileTableEntry *entry );

		void DetermineEntryPaths();

	public:
		bool Open( const std::string &path );

	private:
		FileTableEntry *FindEntry( uint32_t key );

	public:
		void Export( const std::string &destination ) const;
	};
}// namespace core::fs
