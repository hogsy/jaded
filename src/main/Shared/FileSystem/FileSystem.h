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

		typedef uint32_t FileIndex;
		typedef uint32_t DirIndex;

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
		static std::string GetAppDataPath();
		static std::string NormalizePath( std::string path );

		static std::string GetFilenameExtension( const std::string &filename );

		static bool SetWorkingDirectory( const std::string &path );

		void PrintKeyTable() const;

		static bool DoesFileExist( const std::string &path );

		static bool CreateLocalPath( const std::string &path );

		static size_t GetLocalFileSize( const std::string &path );

		bool CreateKeyRepository( const BIG_tdst_BigFile *bf );
		bool ParseKeyRepository( const std::string &path );

		static Key GenerateFileKey( const std::string &path );

		bool IsKeyTablePopulated() const;

		std::vector< FileIndex > GetDirFiles( const std::string &path );

		// A lot of this is dumb, and index-driven, so we can use

		DirIndex CreatePath( const std::string &path );
		DirIndex IndexPath( const std::string &path );
		KeyDir  *GetDirByName( const std::string &path );

		FileIndex GetFileIndexByKey( Key key );
		KeyFile  *GetFileByName( const std::string &path );
		KeyFile  *GetFileByIndex( FileIndex index );

	private:
		void IndexBFSubDirectory( unsigned int curDir );

	public:
		void ClearTables();

	private:
		std::string dataPath;// this is where the data should be stored

		std::map< std::string, DirIndex > dirLookup;// and lookup by name
		std::vector< KeyDir >             directories;

		std::map< std::string, FileIndex > fileLookup;
		std::vector< KeyFile >             files;

		std::map< Key, FileIndex > keys;// table of all key'd files, for lookup

		Key universeKey{ BIG_C_InvalidKey };

	public:
		const Key &GetUniverseKey() const;
	};

	extern FileSystem filesystem;
}// namespace jaded

#else// C interface

uint32_t Jaded_FileSystem_GenerateFileKey( const char *path );
uint32_t Jaded_FileSystem_SearchFileExt( const char *path );

uint32_t Jaded_FileSystem_CreatePath( const char *path );
uint32_t Jaded_FileSystem_LookupDirectory( const char *path );

uint32_t    Jaded_FileSystem_GetFileIndexByKey( uint32_t key );

#endif
