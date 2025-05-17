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
		void ParseKeyRepository( const std::string &path );

		/// <summary>
		///		Geneate a key from the given path.
		/// </summary>
		/// <param name="path">Path of the file; the key is hashed from this.</param>
		/// <returns>Key based on the path. Returns 0 on fail.</returns>
		uint32_t GenerateFileKey( const std::string &path );

		void IndexFile( Key key, const std::string &dir, const std::string &filename );

		/// <summary>
		///		Lookup a file by dir and filename, and return its index.
		/// </summary>
		/// <param name="dir">Directory to lookup.</param>
		/// <param name="filename">Filename to lookup.</param>
		/// <returns>File index on success, otherwise invalid index on fail.</returns>
		Index LookupFile( const std::string &dir, const std::string &filename );

	private:
		void IndexBFSubDirectory( unsigned int curDir );

	private:
		std::string dataPath;// this is where the data should be stored

		struct KeyDir;
		struct KeyFile
		{
			std::string name; // name of the file, without directory (see dir)
			KeyDir     *dir;  // directory it exists under
			Index       index;// its original index in the big file
			Key         key;  // key
		};

		struct KeyDir
		{
			std::string name;
			Index       index;

			std::vector< KeyFile >       files;
			std::map< std::string, Key > fileLookup;
		};

		std::map< Key, KeyFile > fileTable;// table of all key'd files, for lookup

		std::vector< KeyDir >        directories;
		std::map< std::string, Key > dirLookup;// and lookup by name
	};

	extern FileSystem filesystem;
}// namespace jaded

#else// C interface

void     Jaded_FileSystem_CreateKeyRepository( const BIG_tdst_BigFile *bf );
uint32_t Jaded_FileSystem_GenerateFileKey( const char *path );
uint32_t Jaded_FileSystem_SearchFileExt( const char *dir, const char *filename );

#endif
