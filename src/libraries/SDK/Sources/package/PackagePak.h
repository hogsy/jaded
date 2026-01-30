// Created by Mark "hogsy" Sowden, 2023-2026 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#pragma once

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
		uint32_t unk0;
		uint64_t offset;

		std::vector< char > Read( FILE *file ) const;
	};

	struct FileTableEntry
	{
		bool isKeyID;
		union
		{
			uint32_t key;
			char     name[ BIG_C_MaxLenPath ];
		} ident;

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

public:
	bool Open( const std::string &path );

	const FileTableEntry *FindEntry( uint32_t key ) const;

	void Export( const std::string &destination ) const;
};
