// Created by Mark "hogsy" Sowden, 2023-2025 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/

#pragma once

#include <SDL3/SDL.h>

#define JADED_USE_WINMAIN_SDL

namespace jaded
{
	namespace sys
	{
		enum AlertBoxType : uint8_t
		{
			ALERT_BOX_INFO,
			ALERT_BOX_WARNING,
			ALERT_BOX_ERROR,
		};

		enum AlertBoxReturnType : uint8_t
		{
			ALERT_BOX_RETURN_INVALID,

			ALERT_BOX_RETURN_OK,
			ALERT_BOX_RETURN_RETRY,
			ALERT_BOX_RETURN_CONTINUE,
			ALERT_BOX_RETURN_CANCEL,
		};

		AlertBoxReturnType AlertBox( const std::string &message, const std::string &title, AlertBoxType boxType );

		extern int                numLaunchArguments;
		extern const char *const *launchArguments;
		struct LaunchOperations
		{
			bool editorMode{};
			bool popupError{};// Showin added Param for PopUp Script Errors (if off it uses console)
			bool debugConsole{};

			bool forceWindowed{};
			int  forcedWidth{};
			int  forcedHeight{};

			bool                       clean{};
			bool                       cleanBinFiles{};
			std::vector< std::string > wolFiles;
			bool                       cleanByFat{};

			std::string projectFile;

			std::vector< std::string > aiMacros;

			bool portableMode{};// save to the current directory, rather than app data path
		};
		extern LaunchOperations launchOperations;
	}// namespace sys
}// namespace jaded
