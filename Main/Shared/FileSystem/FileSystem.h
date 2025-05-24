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
		};

		struct KeyDir
		{
			std::string name;
			DirIndex    index;

			std::vector< FileIndex > files;
		};

		FileSystem()  = default;
		~FileSystem() = default;

		std::string GetExecutablePath();
		std::string NormalizePath( std::string path );

		std::string GetFilenameExtension( const std::string &filename );

		bool SetWorkingDirectory( const std::string &path );

		void PrintKeyTable();

		bool DoesFileExist( const std::string &path );

		bool CreateLocalPath( const std::string &path );

		size_t GetLocalFileSize( const std::string &path );

		bool ReadFileByIndex( FileIndex index, std::vector< uint8_t > *dst );
		bool ReadFileByName( const std::string &path, std::vector< uint8_t > *dst );

		bool SetProject( const std::string &path );

		bool CreateKeyRepository( const BIG_tdst_BigFile *bf );
		bool ParseKeyRepository( const std::string &path );

		Key GenerateFileKey( const std::string &path );

		const std::vector< FileIndex > &GetDirFiles( const std::string &path );

		// A lot of this is dumb, and index-driven, so we can use

		DirIndex CreatePath( const std::string &path );
		DirIndex IndexPath( const std::string &path );
		KeyDir  *GetDirByName( const std::string &path );

		FileIndex GetFileIndexByKey( Key key );
		KeyFile  *GetFileByName( const std::string &path );
		KeyFile  *GetFileByIndex( FileIndex index );

		std::string GetFilePathByIndex( FileIndex index );

	private:
		void IndexBFSubDirectory( unsigned int curDir );

		void ClearTables();

	private:
		std::string dataPath;// this is where the data should be stored

	private:
		std::map< std::string, DirIndex > dirLookup;// and lookup by name
		std::vector< KeyDir >             directories;

		std::map< std::string, FileIndex > fileLookup;
		std::vector< KeyFile >             files;

		std::map< Key, FileIndex > keys;// table of all key'd files, for lookup
	};

	extern FileSystem filesystem;
}// namespace jaded

#else// C interface

uint32_t Jaded_FileSystem_GenerateFileKey( const char *path );
uint32_t Jaded_FileSystem_SearchFileExt( const char *path );

uint32_t Jaded_FileSystem_CreatePath( const char *path );
uint32_t Jaded_FileSystem_LookupDirectory( const char *path );

uint32_t    Jaded_FileSystem_GetFileIndexByKey( uint32_t key );
const char *Jaded_FileSystem_GetFilePathByIndex( uint32_t index );

#endif
