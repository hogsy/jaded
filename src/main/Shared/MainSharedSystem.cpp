// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include "MainSharedSystem.h"

int jaded::sys::numLaunchArguments             = 0;
const char *const *jaded::sys::launchArguments = nullptr;
jaded::sys::LaunchOperations jaded::sys::launchOperations;

jaded::sys::AlertBoxReturnType jaded::sys::AlertBox( const std::string &message, const std::string &title, AlertBoxType boxType )
{
#if defined( _WIN32 )

	uint32_t flags = 0;
	switch ( boxType )
	{
		default:
			assert( 0 );
			break;
		case ALERT_BOX_ERROR:
			flags |= MB_ICONERROR;
			break;
		case ALERT_BOX_WARNING:
			flags |= MB_ICONEXCLAMATION;
			break;
		case ALERT_BOX_INFO:
			flags |= MB_ICONINFORMATION;
			break;
	}

	int returnVar = MessageBox( nullptr, message.c_str(), title.c_str(), flags );
	switch ( returnVar )
	{
		default:
			assert( 0 );
			return ALERT_BOX_RETURN_INVALID;
		case IDCONTINUE:
		case IDIGNORE:
			return ALERT_BOX_RETURN_CONTINUE;
		case IDOK:
			return ALERT_BOX_RETURN_OK;
		case IDRETRY:
			return ALERT_BOX_RETURN_RETRY;
		case IDNO:
		case IDABORT:
		case IDCANCEL:
			return ALERT_BOX_RETURN_CANCEL;
	}

#else

#	pragma error( "TODO" )

#endif
}
