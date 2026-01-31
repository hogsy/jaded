// Created by Mark "hogsy" Sowden, 2023-2025 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#pragma once

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
		static std::string GetAppDataPath();
		static std::string NormalizePath( std::string path );

		/**
		 * Fetch the extension of the filename, in a lowercase form.
		 * @param filename	Name of the file you want the extension from.
		 * @return			Returns the extension, if found. Will be converted to lowercase.
		 */
		static std::string GetFilenameExtension( const std::string &filename );

		static bool SetWorkingDirectory( const std::string &path );

		void PrintKeyTable() const;

		static bool DoesFileExist( const std::string &path );

		static bool CreateLocalPath( const std::string &path );

		static size_t GetLocalFileSize( const std::string &path );
		static time_t GetLocalFileTimestamp( const std::string &path );

		bool CreateKeyRepository( const BIG_tdst_BigFile *bf );
		bool ParseKeyRepository( const std::string &path );

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
