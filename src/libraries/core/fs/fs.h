// File created for Jaded, the community patched Jade engine

#pragma once

#include "core/core.h"

namespace core::fs
{
	class FileSystem
	{
	public:
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

		static bool DoesLocalFileExist( const std::string &path );

		static bool CreateLocalPath( const std::string &path );

		static size_t GetLocalFileSize( const std::string &path );
		static time_t GetLocalFileTimestamp( const std::string &path );
	};
}// namespace core::fs
