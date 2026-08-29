// File created for Jaded, the community patched Jade engine

#pragma once

#include "../core.h"

namespace core::fs
{
	typedef uint32_t Key;

	typedef uint32_t FileIndex;
	typedef uint32_t DirIndex;

	static constexpr Key       INVALID_KEY   = 0xFFFFFFFF;
	static constexpr FileIndex INVALID_INDEX = 0xFFFFFFFF;

	class Project
	{
		struct KeyFile
		{
			std::string name;   // name of the file, without directory (see dir)
			DirIndex    dir;    // index into the directory table
			FileIndex   index;  // index into the files table
			FileIndex   bfIndex;// used during extraction
			Key         key;    // key

			bool markedForDeletion;

			std::vector< uint8_t > Read( const Project &project ) const;

			std::string GetPath( const Project &project ) const;

			size_t GetSize( const Project &project ) const;
			time_t GetTimestamp( const Project &project ) const;
		};

		struct KeyDir
		{
			std::string name;
			DirIndex    index;

			std::vector< FileIndex > files;
		};

		std::string dataPath;// this is where the data should be stored

		std::map< std::string, DirIndex > dirLookup;// and lookup by name
		std::vector< KeyDir >             directories;

		std::map< std::string, FileIndex > fileLookup;
		std::vector< KeyFile >             files;

		std::map< Key, FileIndex > keys;// table of all key'd files, for lookup

		Key universeKey{ INVALID_KEY };

		void PrintKeyTable() const;

#if 0//TODO: move this out
		bool CreateKeyRepository( const BIG_tdst_BigFile *bf );
#endif
		void ParseKeyRepository( const std::string &path );

		static Key GenerateFileKey( const std::string &path );

		bool IsKeyTablePopulated() const;

		std::vector< FileIndex > GetDirFiles( const std::string &path );

		DirIndex CreatePath( const std::string &path );
		DirIndex IndexPath( const std::string &path );
		KeyDir  *GetDirByName( const std::string &path );
		KeyDir  *GetDirByIndex( DirIndex index );

		FileIndex GetFileIndexByKey( Key key );
		KeyFile  *GetFileByKey( Key key );
		KeyFile  *GetFileByName( const std::string &path );
		KeyFile  *GetFileByIndex( FileIndex index );

#if 0//TODO: move this out
		void IndexBFSubDirectory( unsigned int curDir );
#endif

	public:
		std::string GetFilePath( Key key ) const;

		std::vector< uint8_t > ReadFile( Key key ) const;

		void ClearTables();

		const Key &GetUniverseKey() const;
	};
}// namespace core::fs
