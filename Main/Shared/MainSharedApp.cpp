// Created by Mark "hogsy" Sowden, 2023 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include "MainSharedSystem.h"

#include "BASe/MEMory/MEMpro.h"
#include "ENGine/Sources/ENGinit.h"

#if defined( _WIN32 ) && !defined( NDEBUG )

#	include <DbgHelp.h>

static LONG WINAPI Win32CrashHandler( EXCEPTION_POINTERS *exception )
{
	MessageBox( nullptr, "Encountered an exception, attempting to generate dump!", "Error", MB_OK | MB_ICONERROR );

	HMODULE dbgHelpLib = LoadLibrary( "DBGHELP.DLL" );
	if ( dbgHelpLib == nullptr )
		return EXCEPTION_CONTINUE_SEARCH;

	typedef BOOL( WINAPI * MINIDUMP_WRITE_DUMP )(
	        IN HANDLE hProcess,
	        IN DWORD ProcessId,
	        IN HANDLE hFile,
	        IN MINIDUMP_TYPE DumpType,
	        IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	        OPTIONAL IN PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	        OPTIONAL IN PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL );

	auto MiniDumpWriteDump_ = ( MINIDUMP_WRITE_DUMP ) GetProcAddress( dbgHelpLib, "MiniDumpWriteDump" );
	if ( MiniDumpWriteDump_ == nullptr )
	{
		FreeLibrary( dbgHelpLib );
		return EXCEPTION_CONTINUE_SEARCH;
	}

	MINIDUMP_EXCEPTION_INFORMATION M;
	CHAR Dump_Path[ MAX_PATH ];

	M.ThreadId          = GetCurrentThreadId();
	M.ExceptionPointers = exception;
	M.ClientPointers    = 0;

	GetModuleFileName( nullptr, Dump_Path, sizeof( Dump_Path ) );
	lstrcpy( Dump_Path + lstrlen( Dump_Path ) - 3, "dmp" );

	HANDLE fileDump = CreateFile( Dump_Path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
	if ( fileDump )
	{
		MiniDumpWriteDump_( GetCurrentProcess(), GetCurrentProcessId(), fileDump, MiniDumpNormal, ( exception ) ? &M : nullptr, nullptr, nullptr );
		CloseHandle( fileDump );
	}

	FreeLibrary( dbgHelpLib );
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif

#if defined( _WIN32 )
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow )
#else
int main( int argc, char **argv )
#endif
{
#if defined( _WIN32 ) && !defined( NDEBUG )
	SetUnhandledExceptionFilter( Win32CrashHandler );
#endif

	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		jaded::sys::AlertBox( "SDL Init fail: " + std::string( SDL_GetError() ),
		                      "Jaded Error",
		                      jaded::sys::ALERT_BOX_ERROR );
		return EXIT_FAILURE;
	}

	MEMpro_Init();
	ENG_InitApplication();

	// blah blah

	ENG_CloseApplication();

	return EXIT_SUCCESS;
}
