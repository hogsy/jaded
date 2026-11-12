// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include <iomanip>
#include <sstream>

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
#	include <winhttp.h>
#	include <Windowsx.h>
#endif

#include "MainSharedSystem.h"

#include "core/fs/fs.h"
#include "core/core_logger.h"

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
#	include "Res/Res.h"
#endif

#include "BASe/MEMory/MEMpro.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "SDK/Sources/BIGfiles/BIGopen.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDInterface.h"
#include "AppVersion.h"

#include "ImGuiInterface.h"

#include "Profiler.h"

#if defined( _MSC_VER )
#	pragma comment( lib, "SDL3.lib" )
#endif

jaded::sys::Profiler jaded::sys::profiler;

static SDL_Window   *sdlWindow;
static SDL_GLContext sdlGLContext;

/******************************************************************/
/******************************************************************/

void jaded::sys::Profiler::StartProfiling( const std::string &set )
{
	if ( !isActive )
	{
		return;
	}

	auto i = profSets.find( set );
	if ( i == profSets.end() )
	{
		profSets.emplace( set, Profile() );
		return;
	}

	i->second.Start();
}

double jaded::sys::Profiler::EndProfiling( const std::string &set )
{
	if ( !isActive )
	{
		return 0;
	}

	auto i = profSets.find( set );
	assert( i != profSets.end() );
	i->second.End();

	return i->second.GetTimeTaken();
}

// The below should work with the old BeginRaster / EndRaster macros

extern "C" void Jaded_Profiler_StartProfiling( unsigned int set )
{
	jaded::sys::profiler.StartProfiling( std::to_string( set ) );
}

extern "C" void Jaded_Profiler_EndProfiling( unsigned int set )
{
	jaded::sys::profiler.EndProfiling( std::to_string( set ) );
}

/******************************************************************/
/******************************************************************/

static std::string logPath;

static void *nativeWindowHandle;

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS

#	include <DbgHelp.h>

static char *CreateCrashReporterEnvironment( const char *dump_path )
{
	/* Create a copy of our environment block with the JADED_LOG_PATH variable added. */

	const char *our_env = GetEnvironmentStrings();

	size_t crash_reporter_env_size = 1;

	for ( const char *s = our_env; *s != '\0'; )
	{
		size_t ss = strlen( s ) + 1;

		crash_reporter_env_size += ss;
		s += ss;
	}

	crash_reporter_env_size += strlen( "JADED_LOG_PATH=" );
	crash_reporter_env_size += strlen( dump_path );
	crash_reporter_env_size += 1;

	/* We are called when the process is in an unknown state - there could be memory corruption
	 * which would make the heap allocation functions unsafe, so we instead ask the kernel to
	 * map us some memory to build the environment block for the crash reporter process.
	 *
	 * This memory is never freed, but then again, the game is in the process of dying anyway.
	*/

	const HANDLE crash_reporter_env_map = CreateFileMapping(
	        INVALID_HANDLE_VALUE,
	        nullptr,
	        PAGE_READWRITE,
	        0,
	        crash_reporter_env_size,
	        nullptr );
	if ( crash_reporter_env_map == nullptr )
	{
		return nullptr;
	}

	char *crash_reporter_env = ( char * ) ( MapViewOfFile( crash_reporter_env_map, ( FILE_MAP_READ | FILE_MAP_WRITE ), 0, 0, crash_reporter_env_size ) );
	if ( crash_reporter_env == nullptr )
	{
		return nullptr;
	}

	crash_reporter_env_size = 0;

	for ( const char *s = our_env; *s != '\0'; )
	{
		size_t ss = strlen( s ) + 1;

		strcpy( ( crash_reporter_env + crash_reporter_env_size ), s );
		crash_reporter_env_size += ss;
		s += ss;
	}

	strcpy( ( crash_reporter_env + crash_reporter_env_size ), "JADED_LOG_PATH=" );
	crash_reporter_env_size += strlen( "JADED_LOG_PATH=" );

	strcpy( ( crash_reporter_env + crash_reporter_env_size ), dump_path );
	crash_reporter_env_size += strlen( dump_path ) + 1;

	/* Add terminator at end of environment block. */
	crash_reporter_env[ crash_reporter_env_size++ ] = '\0';

	return crash_reporter_env;
}

static LONG WINAPI Win32CrashHandler( EXCEPTION_POINTERS *exception )
{
	const HMODULE dbgHelpLib = LoadLibrary( "DBGHELP.DLL" );
	if ( dbgHelpLib == nullptr )
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	typedef BOOL( WINAPI * MINIDUMP_WRITE_DUMP )(
	        IN HANDLE                                                hProcess,
	        IN DWORD                                                 ProcessId,
	        IN HANDLE                                                hFile,
	        IN MINIDUMP_TYPE                                         DumpType,
	        IN CONST PMINIDUMP_EXCEPTION_INFORMATION                 ExceptionParam,
	        OPTIONAL IN PMINIDUMP_USER_STREAM_INFORMATION            UserStreamParam,
	        OPTIONAL IN PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL );

	auto MiniDumpWriteDump_ = ( MINIDUMP_WRITE_DUMP ) GetProcAddress( dbgHelpLib, "MiniDumpWriteDump" );
	if ( MiniDumpWriteDump_ == nullptr )
	{
		FreeLibrary( dbgHelpLib );
		return EXCEPTION_CONTINUE_SEARCH;
	}

	MINIDUMP_EXCEPTION_INFORMATION M;
	CHAR                           Exe_Path[ MAX_PATH ], Dump_Path[ MAX_PATH ];

	M.ThreadId          = GetCurrentThreadId();
	M.ExceptionPointers = exception;
	M.ClientPointers    = 0;

	GetModuleFileName( nullptr, Exe_Path, sizeof( Exe_Path ) );

	lstrcpy( Dump_Path, Exe_Path );
	lstrcpy( Dump_Path + lstrlen( Dump_Path ) - 3, "dmp" );

	const HANDLE fileDump = CreateFile( Dump_Path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
	if ( fileDump )
	{
		MiniDumpWriteDump_( GetCurrentProcess(), GetCurrentProcessId(), fileDump, MiniDumpNormal, ( exception ) ? &M : nullptr, nullptr, nullptr );
		CloseHandle( fileDump );
	}

	char *crash_reporter_env = CreateCrashReporterEnvironment( logPath.c_str() );

	STARTUPINFO si = {};
	si.cb          = sizeof( si );

	PROCESS_INFORMATION pi;

	if ( CreateProcess(
	             Exe_Path,
	             nullptr,
	             nullptr,
	             nullptr,
	             FALSE,
	             0,
	             crash_reporter_env,
	             nullptr,
	             &si,
	             &pi ) )
	{
		CloseHandle( pi.hThread );
		CloseHandle( pi.hProcess );
	}
	else
	{
		MessageBox( nullptr, "Encountered an exception, launching the crash reporter failed!", "Error", MB_OK | MB_ICONERROR );
	}

	FreeLibrary( dbgHelpLib );

	return EXCEPTION_CONTINUE_SEARCH;
}

#endif

extern "C" bool jaded_enableLegacyBehaviour = false;
static void     ParseStartupParameters()
{
	assert( jaded::sys::launchArguments != nullptr );

	for ( int i = 0; i < jaded::sys::numLaunchArguments; ++i )
	{
		if ( *jaded::sys::launchArguments[ i ] != '/' )
			continue;

		if ( jaded::sys::launchArguments[ i ] == std::string( "/editor" ) )
		{
			jaded::sys::launchOperations.editorMode = true;
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/portable" ) )
		{
			jaded::sys::launchOperations.portableMode = true;
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/popuperror" ) )// Showin added Param for PopUp Script Errors (if off it uses console)
		{
			jaded::sys::launchOperations.popupError = true;
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/console" ) )
		{
			jaded::sys::launchOperations.debugConsole = true;
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/window" ) )
		{
			jaded::sys::launchOperations.forceWindowed = true;
			continue;
		}

		if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/width", 6 ) == 0 )
		{
			jaded::sys::launchOperations.forcedWidth = strtol( jaded::sys::launchArguments[ i ] + 7, nullptr, 10 );
			continue;
		}

		if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/height", 7 ) == 0 )
		{
			jaded::sys::launchOperations.forcedHeight = strtol( jaded::sys::launchArguments[ i ] + 8, nullptr, 10 );
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/uncap" ) )
		{
			ENG_gb_LimitFPS = false;
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/profile" ) )
		{
			jaded::sys::profiler.SetActive( true );
			ENG_gb_LimitFPS = false;
			continue;
		}

		if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/bf", 3 ) == 0 )
		{
			if ( ++i >= jaded::sys::numLaunchArguments )
			{
				break;
			}

			jaded::sys::launchOperations.projectFile = jaded::sys::launchArguments[ i ];
			continue;
		}

		if ( jaded::sys::launchArguments[ i ] == std::string( "/legacy" ) )
		{
			jaded_enableLegacyBehaviour = true;
			continue;
		}

		if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/log", 4 ) == 0 )
		{
			if ( ++i >= jaded::sys::numLaunchArguments )
			{
				break;
			}

			logPath = jaded::sys::launchArguments[ i ];
			continue;
		}
	}
}

static SDL_Window *CreateSDLWindow()
{
	int flags = SDL_WINDOW_RESIZABLE;
	if ( !jaded::sys::launchOperations.forceWindowed )
		flags |= SDL_WINDOW_FULLSCREEN;

	int            numDisplays;
	SDL_DisplayID *displays = SDL_GetDisplays( &numDisplays );

	int                    w, h;
	const SDL_DisplayMode *displayMode;
	if ( ( displayMode = SDL_GetDesktopDisplayMode( displays[ 0 ] ) ) != nullptr )
	{
		w = displayMode->w;
		h = displayMode->h;
	}
	else
	{
		printf( "Failed to get desktop display mode: %s\n", SDL_GetError() );
		w = 1024;
		h = 768;
	}

	SDL_free( displays );

	if ( jaded::sys::launchOperations.forcedWidth > 0 ) w = jaded::sys::launchOperations.forcedWidth;
	if ( jaded::sys::launchOperations.forcedHeight > 0 ) h = jaded::sys::launchOperations.forcedHeight;

	static const std::string title = jaded::APP_NAME + std::string( " " ) + jaded::GetReleaseVersion();

	sdlWindow = SDL_CreateWindow( title.c_str(), w, h, flags );
	if ( sdlWindow == nullptr )
	{
		return nullptr;
	}

	if ( !jaded::sys::launchOperations.forceWindowed )
	{
		SDL_SetWindowFullscreen( sdlWindow, true );
	}

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
	nativeWindowHandle = SDL_GetPointerProperty( SDL_GetWindowProperties( sdlWindow ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr );
#endif

	return sdlWindow;
}

static void InitializeDisplay()
{
	MAI_gh_MainWindow                   = ( HWND ) ( MAI_gst_MainHandles.h_DisplayWindow = nativeWindowHandle );
	MAI_gst_MainHandles.pst_DisplayData = GDI_fnpst_CreateDisplayData();
	GDI_gpst_CurDD                      = MAI_gst_MainHandles.pst_DisplayData;

	GDI_fnl_InitInterface( &MAI_gst_MainHandles.pst_DisplayData->st_GDI, 1 );

	MAI_gst_MainHandles.pst_DisplayData->pv_SpecificData = MAI_gst_MainHandles.pst_DisplayData->st_GDI.pfnpv_InitDisplay();
	GDI_AttachDisplay( MAI_gst_MainHandles.pst_DisplayData, MAI_gst_MainHandles.h_DisplayWindow );

#ifdef RASTERS_ON

	GDI_Rasters_Init( MAI_gst_MainHandles.pst_DisplayData->pst_Raster, "Display Data" );

#endif

	MAI_gst_MainHandles.pst_DisplayData->uc_EngineCamera = TRUE;
	MAI_gst_MainHandles.pst_DisplayData->ul_DrawMask |= GDI_Cul_DM_NoAutoClone;

	MAI_gst_MainHandles.pst_DisplayData->st_ScreenFormat.ul_Flags = GDI_Cul_SFF_OccupyAll;
}

static void ShutdownDisplay()
{
	if ( MAI_gst_MainHandles.pst_DisplayData == nullptr )
		return;

	GDI_DetachDisplay( MAI_gst_MainHandles.pst_DisplayData );
	MAI_gst_MainHandles.pst_DisplayData->st_GDI.pfnv_DesinitDisplay( MAI_gst_MainHandles.pst_DisplayData->pv_SpecificData );
	GDI_fnv_DestroyDisplayData( MAI_gst_MainHandles.pst_DisplayData );
}

static void OpenBFCallback( void *userdata, const char *const *filelist, int )
{
	bool *status = static_cast< bool * >( userdata );
	if ( filelist[ 0 ] == nullptr )
	{
		*status = true;
		return;
	}

	if ( !BIG_Open( filelist[ 0 ] ) )
	{
		*status = true;
		return;
	}

	snprintf( MAI_gst_InitStruct.asz_ProjectName, sizeof( MAI_gst_InitStruct.asz_ProjectName ), "%s", filelist[ 0 ] );
	*status = true;
}

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
extern int EDI_EditorWin32Execution( HINSTANCE );
#endif

static int core_main( int argc, char **argv )
{
	const char *dump = getenv( "JADED_LOG_PATH" );
	if ( dump != nullptr )
	{
		const time_t t         = time( nullptr );
		const tm     localTime = *localtime( &t );

		std::ostringstream ss;
		ss << "crash (" << std::put_time( &localTime, "%d-%m-%Y %H-%M-%S" ) << ").txt";
		const std::string crashLogFilename = ss.str();

		if ( !core::fs::FileSystem::CopyFile( dump, crashLogFilename ) )
		{
			const std::string appData = core::fs::FileSystem::GetAppDataPath( jaded::APP_NAME );
			core::Logger::GetInstance().DumpOutput( appData + "/" + crashLogFilename );
		}

		return EXIT_SUCCESS;
	}

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
	SetUnhandledExceptionFilter( Win32CrashHandler );
#endif

	jaded::sys::numLaunchArguments = argc;
	jaded::sys::launchArguments    = argv;

	ParseStartupParameters();

	if ( !logPath.empty() && !core::Logger::GetInstance().SetOutputFile( logPath ) )
	{
		CORE_LOGGER_WARN( "Failed to set output file for logs (%s)!\n", logPath.c_str() );
		logPath.clear();
	}

	if ( !SDL_Init( SDL_INIT_GAMEPAD | SDL_INIT_VIDEO | SDL_INIT_EVENTS ) )
	{
		const std::string msg = "SDL initialisation failed: " + std::string( SDL_GetError() );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Jaded Error", msg.c_str(), nullptr );
		return EXIT_FAILURE;
	}

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
	if ( jaded::sys::launchOperations.debugConsole )
	{
		AllocConsole();
		FILE *tmp;
		freopen_s( &tmp, "CONIN$", "r", stdin );
		freopen_s( &tmp, "CONOUT$", "w", stderr );
		freopen_s( &tmp, "CONOUT$", "w", stdout );
	}
#endif

	std::string localAppData;
	if ( !jaded::sys::launchOperations.portableMode )
	{
		localAppData = core::fs::FileSystem::GetAppDataPath( jaded::APP_NAME );
		if ( localAppData.empty() )
		{
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Jaded Error", "Failed to get local app data path!", nullptr );
			return EXIT_FAILURE;
		}
	}
	else
	{
		localAppData = ".";
	}

	if ( !core::fs::FileSystem::CreateLocalPath( localAppData ) )
	{
		const std::string msg = "Failed to create local app data path (" + localAppData + ")!";
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Jaded Error", msg.c_str(), nullptr );
		return EXIT_FAILURE;
	}

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
	// hogsy: for now we'll only support editor functionality under win32
	if ( jaded::sys::launchOperations.editorMode )
	{
		const HINSTANCE instance = GetModuleHandle( nullptr );
		return EDI_EditorWin32Execution( instance );
	}
#endif

	if ( CreateSDLWindow() == nullptr )
	{
		const std::string msg = "SDL Window fail: " + std::string( SDL_GetError() );
		SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Jaded Error", msg.c_str(), nullptr );
		return EXIT_FAILURE;
	}

	MEMpro_Init();
	MEMpro_StartMemRaster();

	ENG_InitApplication();

	// Default big file name
	const char *projectFile = "Rayman4.bf";
	if ( !jaded::sys::launchOperations.projectFile.empty() )
	{
		projectFile = jaded::sys::launchOperations.projectFile.c_str();
	}

	snprintf( MAI_gst_InitStruct.asz_ProjectName, sizeof( MAI_gst_InitStruct.asz_ProjectName ), "%s", projectFile );
	if ( !BIG_Open( MAI_gst_InitStruct.asz_ProjectName ) )
	{
		static constexpr SDL_DialogFileFilter FILTERS[] = {
		        { "BIG Files", "bf" }
        };
		static constexpr unsigned int NUM_FILTERS = std::size( FILTERS );

		bool status = false;
		SDL_ShowOpenFileDialog( OpenBFCallback, &status, nullptr, FILTERS, NUM_FILTERS, nullptr, false );

		while ( !status )
		{
			SDL_PumpEvents();
		}

		if ( BIG_gst.h_CLibFileHandle == nullptr )
		{
			SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Jaded Error", "Please select a valid BF!", nullptr );
			return EXIT_FAILURE;
		}
	}

	InitializeDisplay();

#ifdef IMGUI_USE_SDL3
	ImGuiInterface_Initialize( sdlWindow );
#endif

	ENG_InitEngine();

	ENG_Loop();

	WOR_Universe_Close( 0 );

	ShutdownDisplay();

	ENG_CloseEngine();
	ENG_CloseApplication();

	ImGuiInterface_Shutdown();

	SDL_DestroyWindow( sdlWindow );

#if CORE_SYSTEM == CORE_SYSTEM_WINDOWS
	if ( jaded::sys::launchOperations.debugConsole )
	{
		FreeConsole();
	}
#endif

	return EXIT_SUCCESS;
}

CORE_SYSTEM_IMPLEMENT_MAIN()
