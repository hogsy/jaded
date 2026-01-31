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

	class FileSystem
	{
	public:
		struct KeyFile
		{
			std::string name;   // name of the file, without directory (see dir)
			DirIndex    dir;    // index into the directory table
			FileIndex   index;  // index into the files table
			FileIndex   bfIndex;// used during extraction
			Key         key;    // key

			bool markedForDeletion;

			std::vector< uint8_t > Read() const;

			std::string GetPath() const;

			size_t GetSize() const;
			time_t GetTimestamp() const;
		};

		struct KeyDir
		{
			std::string name;
			DirIndex    index;

			std::vector< FileIndex > files;
		};

		FileSystem()  = default;
		~FileSystem() = default;

		static std::string GetExecutablePath();
		static std::string GetAppDataPath( const std::string &appName );
		static std::string NormalizePath( std::string path );

		/**
		 * Fetch the extension of the filename, in a lowercase form.
		 * @param filename	Name of the file you want the extension from.
		 * @return			Returns the extension, if found. Will be converted to lowercase.
		 */
		static std::string GetFilenameExtension( const std::string &filename );

		static bool SetWorkingDirectory( const std::string &path );

		void PrintKeyTable() const;

		static bool DoesLocalFileExist( const std::string &path );

		static bool CreateLocalPath( const std::string &path );

		static size_t GetLocalFileSize( const std::string &path );
		static time_t GetLocalFileTimestamp( const std::string &path );

#if 0//TODO: move this out
		bool CreateKeyRepository( const BIG_tdst_BigFile *bf );
#endif
		void ParseKeyRepository( const std::string &path );

		static Key GenerateFileKey( const std::string &path );

		bool IsKeyTablePopulated() const;

		std::vector< FileIndex > GetDirFiles( const std::string &path );

		// A lot of this is dumb, and index-driven, so we can use

		DirIndex CreatePath( const std::string &path );
		DirIndex IndexPath( const std::string &path );
		KeyDir  *GetDirByName( const std::string &path );
		KeyDir  *GetDirByIndex( DirIndex index );

		FileIndex GetFileIndexByKey( Key key );
		KeyFile  *GetFileByKey( Key key );
		KeyFile  *GetFileByName( const std::string &path );
		KeyFile  *GetFileByIndex( FileIndex index );

	private:
#if 0//TODO: move this out
		void IndexBFSubDirectory( unsigned int curDir );
#endif

	public:
		void ClearTables();

	private:
		std::string dataPath;// this is where the data should be stored

		std::map< std::string, DirIndex > dirLookup;// and lookup by name
		std::vector< KeyDir >             directories;

		std::map< std::string, FileIndex > fileLookup;
		std::vector< KeyFile >             files;

		std::map< Key, FileIndex > keys;// table of all key'd files, for lookup

		Key universeKey{ INVALID_KEY };

	public:
		const Key &GetUniverseKey() const;
	};

	extern FileSystem filesystem;
}// namespace core::fs
