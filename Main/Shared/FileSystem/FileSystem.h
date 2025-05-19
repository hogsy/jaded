// Created by Mark "hogsy" Sowden, 2023-2025 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#pragma once

#if defined( __cplusplus )

namespace jaded
{
	class FileSystem
	{
	public:
		typedef uint32_t Key;
		typedef uint32_t Index;

		FileSystem()  = default;
		~FileSystem() = default;

		std::string GetExecutablePath();
		std::string NormalizePath( std::string path );

		void PrintKeyTable();

		bool DoesFileExist( const std::string &path );

		bool CreatePath( const std::string &path );

		void CreateKeyRepository( const BIG_tdst_BigFile *bf );
		bool ParseKeyRepository( const std::string &path );

		uint32_t GenerateFileKey( const std::string &path );

		Index LookupFile( const std::string &path );

	private:
		void IndexBFSubDirectory( unsigned int curDir );

	private:
		std::string dataPath;// this is where the data should be stored

		struct KeyFile
		{
			std::string name;   // name of the file, without directory (see dir)
			Index       dir;    // index into the directory table
			Index       index;  // index into the files table
			Index       bfIndex;// used during extraction
			Key         key;    // key

			bool markedForDeletion;
		};

		struct KeyDir
		{
			std::string name;
			Index       index;

			std::vector< Index > files;
		};

		std::map< std::string, Index > dirLookup;// and lookup by name
		std::vector< KeyDir >          directories;

		std::map< std::string, Index > fileLookup;
		std::vector< KeyFile >         files;

		std::map< Key, Index > keys;// table of all key'd files, for lookup
	};

	extern FileSystem filesystem;
}// namespace jaded

#else// C interface

void     Jaded_FileSystem_CreateKeyRepository( const BIG_tdst_BigFile *bf );
uint32_t Jaded_FileSystem_GenerateFileKey( const char *path );
uint32_t Jaded_FileSystem_SearchFileExt( const char *path );

#endif
