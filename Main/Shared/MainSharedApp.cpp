// Created by Mark "hogsy" Sowden, 2023 <hogsy@oldtimes-software.com>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include "MainSharedSystem.h"

#include "BASe/MEMory/MEMpro.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "SDK/Sources/BIGfiles/BIGopen.h"
#include "GDInterface/GDIrasters.h"

static SDL_Window *sdlWindow;
static SDL_GLContext sdlGLContext;

#if defined( _WIN32 )

static HWND nativeWindowHandle;

#endif

#if defined( _WIN32 )

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

static void ParseStartupParameters()
{
	assert( jaded::sys::launchArguments != nullptr );

	for ( int i = 0; i < jaded::sys::numLaunchArguments; ++i )
	{
		if ( *jaded::sys::launchArguments[ i ] != '/' )
			continue;

		if ( SDL_strcasecmp( jaded::sys::launchArguments[ i ], "/editor" ) == 0 )
		{
			jaded::sys::launchOperations.editorMode = true;
			continue;
		}
		else if ( SDL_strcasecmp( jaded::sys::launchArguments[ i ], "/console" ) == 0 )
		{
			jaded::sys::launchOperations.debugConsole = true;
			continue;
		}
	}
}

static SDL_Window *CreateSDLWindow()
{
	int flags = SDL_WINDOW_OPENGL;
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 3 );

	sdlWindow = SDL_CreateWindow( "Jaded", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, flags );
	if ( sdlWindow == nullptr )
		return nullptr;

	sdlGLContext = SDL_GL_CreateContext( sdlWindow );
	if ( sdlGLContext == nullptr )
		return nullptr;

	SDL_GL_MakeCurrent( sdlWindow, sdlGLContext );

#if defined( _WIN32 )

	SDL_SysWMinfo info;
	SDL_VERSION( &info.version );
	SDL_GetWindowWMInfo( sdlWindow, &info );
	nativeWindowHandle = info.info.win.window;

#endif

	return sdlWindow;
}

static void InitializeDisplay()
{
	MAI_gst_MainHandles.h_DisplayWindow = nativeWindowHandle;
	MAI_gst_MainHandles.pst_DisplayData = GDI_fnpst_CreateDisplayData();
	GDI_gpst_CurDD                      = MAI_gst_MainHandles.pst_DisplayData;

	GDI_fnl_InitInterface( &MAI_gst_MainHandles.pst_DisplayData->st_GDI, 1 );

	MAI_gst_MainHandles.pst_DisplayData->pv_SpecificData = MAI_gst_MainHandles.pst_DisplayData->st_GDI.pfnpv_InitDisplay();
	GDI_AttachDisplay( MAI_gst_MainHandles.pst_DisplayData, MAI_gst_MainHandles.h_DisplayWindow );

#ifdef RASTERS_ON

	GDI_Rasters_Init( MAI_gst_MainHandles.pst_DisplayData->pst_Raster, "Display Data" );

#endif
}

static void ShutdownDisplay()
{
	if ( MAI_gst_MainHandles.pst_DisplayData == nullptr )
		return;

	GDI_DetachDisplay( MAI_gst_MainHandles.pst_DisplayData );
	MAI_gst_MainHandles.pst_DisplayData->st_GDI.pfnv_DesinitDisplay( MAI_gst_MainHandles.pst_DisplayData->pv_SpecificData );
	GDI_fnv_DestroyDisplayData( MAI_gst_MainHandles.pst_DisplayData );
}

#if defined( _WIN32 )
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow )
#else
int main( int argc, char **argv )
#endif
{
#if defined( _WIN32 )

	jaded::sys::numLaunchArguments = __argc;
	jaded::sys::launchArguments    = __argv;

	SetUnhandledExceptionFilter( Win32CrashHandler );

#else

	jaded::sys::numLaunchArguments = argc;
	jaded::sys::launchArguments    = argv;

#endif

	ParseStartupParameters();

#if defined( _WIN32 )

	if ( jaded::sys::launchOperations.debugConsole )
	{
		AllocConsole();
		FILE *tmp;
		freopen_s( &tmp, "CONIN$", "r", stdin );
		freopen_s( &tmp, "CONOUT$", "w", stderr );
		freopen_s( &tmp, "CONOUT$", "w", stdout );
	}

#endif

	if ( jaded::sys::launchOperations.editorMode )
	{
	}

	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
	{
		jaded::sys::AlertBox( "SDL Init fail: " + std::string( SDL_GetError() ),
		                      "Jaded Error",
		                      jaded::sys::ALERT_BOX_ERROR );
		return EXIT_FAILURE;
	}

	if ( CreateSDLWindow() == nullptr )
	{
		jaded::sys::AlertBox( "SDL Window fail: " + std::string( SDL_GetError() ),
		                      "Jaded Error",
		                      jaded::sys::ALERT_BOX_ERROR );
	}

	MEMpro_Init();
	MEMpro_StartMemRaster();

	ENG_InitApplication();

	// Default big file name
	strcpy( MAI_gst_InitStruct.asz_ProjectName, "Rayman4.bf" );
	if ( !BIG_Open( MAI_gst_InitStruct.asz_ProjectName ) )
		return EXIT_FAILURE;

	InitializeDisplay();

	ENG_InitEngine();

	ENG_Loop();

	WOR_Universe_Close( 0 );

	ENG_CloseEngine();
	ENG_CloseApplication();

	SDL_DestroyWindow( sdlWindow );

#if defined( _WIN32 ) && !defined( NDEBUG )

	if ( jaded::sys::launchOperations.debugConsole )
		FreeConsole();

#endif

	return EXIT_SUCCESS;
}
