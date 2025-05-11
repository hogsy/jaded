// Created by Mark "hogsy" Sowden, 2023-2025 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#pragma once

#if defined( __cplusplus )

namespace jaded
{
	class FileSystem
	{
	public:
		FileSystem()  = default;
		~FileSystem() = default;

		std::string GetExecutablePath();
		std::string NormalizePath( std::string path );

		bool CreatePath( const std::string &path );

		void CreateKeyRepository( const BIG_tdst_BigFile *bf );

	private:
		void IndexBFSubDirectory( unsigned int curDir );

	private:
		struct KeyFile
		{
			std::string  filename;// should map to local file
			std::string  dir;     // directory it exists under
			unsigned int index;   // its original index in the big file
			unsigned int key;     // key
		};

		std::string                       dataPath; // this is where the data should be stored
		std::map< unsigned int, KeyFile > fileTable;// table of all key'd files, for lookup
	};

	extern FileSystem filesystem;
}// namespace jaded

#else// C interface

void Jaded_FileSystem_CreateKeyRepository( const BIG_tdst_BigFile *bf );

#endif
