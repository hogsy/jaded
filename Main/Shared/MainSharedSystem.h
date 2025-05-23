// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@oldtimes-software.com>
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

		extern int numLaunchArguments;
		extern const char *const *launchArguments;
		struct LaunchOperations
		{
			bool editorMode{ false };
			bool popupError{ false }; // Showin added Param for PopUp Script Errors (if off it uses console)
			bool debugConsole{ false };

			bool forceWindowed{ false };
			int forcedWidth{ 0 };
			int forcedHeight{ 0 };

			bool clean{ false };
			bool cleanBinFiles{ false };
			std::vector< std::string > wolFiles;
			bool cleanByFat{ false };

			std::string projectFile;

			std::vector< std::string > aiMacros;
		};
		extern LaunchOperations launchOperations;
	}// namespace sys
}// namespace jaded
