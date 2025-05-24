// Created by Mark "hogsy" Sowden, 2023-2024 <hogsy@snortysoft.net>
// https://oldtimes-software.com/jaded/

#include "Precomp.h"

#include <winhttp.h>
#include <Windowsx.h>

#include "MainSharedSystem.h"
#include "Res/Res.h"

#include "BASe/MEMory/MEMpro.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "SDK/Sources/BIGfiles/BIGopen.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDInterface.h"

#include "ImGuiInterface.h"

#include "Profiler.h"
#include "FileSystem/FileSystem.h"

jaded::sys::Profiler jaded::sys::profiler;

static SDL_Window   *sdlWindow;
static SDL_GLContext sdlGLContext;

//#define USE_SDL_GL_CONTEXT

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

#if defined( _WIN32 )

extern int EDI_EditorWin32Execution( HINSTANCE );

static HWND nativeWindowHandle;

#	include <DbgHelp.h>

static char *CreateCrashReporterEnvironment( const char *dump_path )
{
	/* Create a copy of our environment block with the JADED_CRASH_DUMP_PATH variable added. */

	const char *our_env = GetEnvironmentStrings();

	size_t crash_reporter_env_size = 1;

	for ( const char *s = our_env; *s != '\0'; )
	{
		size_t ss = strlen( s ) + 1;

		crash_reporter_env_size += ss;
		s += ss;
	}

	crash_reporter_env_size += strlen( "JADED_CRASH_DUMP_PATH=" );
	crash_reporter_env_size += strlen( dump_path );
	crash_reporter_env_size += 1;

	/* We are called when the process is in an unknown state - there could be memory corruption
	 * which would make the heap allocation functions unsafe, so we instead ask the kernel to
	 * map us some memory to build the environment block for the crash reporter process.
	 *
	 * This memory is never freed, but then again, the game is in the process of dying anyway.
	*/

	HANDLE crash_reporter_env_map = CreateFileMapping(
	        INVALID_HANDLE_VALUE,
	        NULL,
	        PAGE_READWRITE,
	        0,
	        crash_reporter_env_size,
	        NULL );

	if ( crash_reporter_env_map == NULL )
	{
		return NULL;
	}

	char *crash_reporter_env = ( char * ) ( MapViewOfFile( crash_reporter_env_map, ( FILE_MAP_READ | FILE_MAP_WRITE ), 0, 0, crash_reporter_env_size ) );
	if ( crash_reporter_env == NULL )
	{
		return NULL;
	}

	crash_reporter_env_size = 0;

	for ( const char *s = our_env; *s != '\0'; )
	{
		size_t ss = strlen( s ) + 1;

		strcpy( ( crash_reporter_env + crash_reporter_env_size ), s );
		crash_reporter_env_size += ss;
		s += ss;
	}

	strcpy( ( crash_reporter_env + crash_reporter_env_size ), "JADED_CRASH_DUMP_PATH=" );
	crash_reporter_env_size += strlen( "JADED_CRASH_DUMP_PATH=" );

	strcpy( ( crash_reporter_env + crash_reporter_env_size ), dump_path );
	crash_reporter_env_size += strlen( dump_path ) + 1;

	/* Add terminator at end of environment block. */
	crash_reporter_env[ crash_reporter_env_size++ ] = '\0';

	return crash_reporter_env;
}

static LONG WINAPI Win32CrashHandler( EXCEPTION_POINTERS *exception )
{
	HMODULE dbgHelpLib = LoadLibrary( "DBGHELP.DLL" );
	if ( dbgHelpLib == nullptr )
		return EXCEPTION_CONTINUE_SEARCH;

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

	HANDLE fileDump = CreateFile( Dump_Path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
	if ( fileDump )
	{
		MiniDumpWriteDump_( GetCurrentProcess(), GetCurrentProcessId(), fileDump, MiniDumpNormal, ( exception ) ? &M : nullptr, nullptr, nullptr );
		CloseHandle( fileDump );
	}

	char *crash_reporter_env = CreateCrashReporterEnvironment( Dump_Path );

	STARTUPINFO si;
	memset( &si, 0, sizeof( si ) );
	si.cb = sizeof( si );

	PROCESS_INFORMATION pi;

	if ( CreateProcess(
	             Exe_Path,
	             NULL,
	             NULL,
	             NULL,
	             FALSE,
	             0,
	             crash_reporter_env,
	             NULL,
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

static const wchar_t *JADED_CRASH_REPORT_HOST = L"www.solemnwarning.net";
static const wchar_t *JADED_CRASH_REPORT_PATH = L"/jaded-crash.cgi";

static bool Win32SendCrashReport( const std::string &details, const char *dump_path )
{
	/* Read in the crash dump. */

	FILE *dump_fh = fopen( dump_path, "rb" );
	if ( dump_fh == NULL )
	{
		return false;
	}

	std::string dump_data;

	std::vector< char > read_buf( 8192 );
	size_t              read_len;

	while ( ( read_len = fread( read_buf.data(), 1, read_buf.size(), dump_fh ) ) > 0 )
	{
		dump_data.insert( dump_data.end(), read_buf.begin(), std::next( read_buf.begin(), read_len ) );
	}

	if ( ferror( dump_fh ) )
	{
		fclose( dump_fh );
		return false;
	}

	read_buf.clear();
	read_buf.shrink_to_fit();

	fclose( dump_fh );
	dump_fh = NULL;

	/* Brute force loop of possible alphanumeric boundary strings to find one that doesn't
	 * appear in any of the data we want to submit.
	*/

	std::string boundary( 70, '0' );

	while ( dump_data.find( "--" + boundary ) != std::string::npos || details
	                                                                                  .find( "--" + boundary ) != std::string::npos )
	{
		bool bi_done = false;

		for ( size_t i = 0; i < boundary.length() && !bi_done; ++i )
		{
			switch ( boundary[ i ] )
			{
				default:
					boundary[ i ] += 1;
					bi_done = true;
					break;

				case '9':
					boundary[ i ] = 'A';
					bi_done       = true;
					break;

				case 'Z':
					boundary[ i ] = 'a';
					bi_done       = true;
					break;

				case 'z':
					boundary[ i ] = '0';
					break;
			}

			if ( !bi_done && i == ( boundary.length() - 1 ) )
			{
				return false;
			}
		}
	}

	std::wstring wide_boundary( boundary.begin(), boundary.end() );

	/* Pack the crash dump and details into a multipart/form-data body. */

	std::string request_body =
	        "--" + boundary + "\r\n" +
	        "Content-Disposition: form-data; name=\"details\"\r\n" +
	        "\r\n" +
	        details + "\r\n" +
	        "--" + boundary + "\r\n" +
	        "Content-Disposition: form-data; name=\"dump\"\r\n" +
	        "Content-Type: application/octet-stream\r\n" +
	        "\r\n" +
	        dump_data + "\r\n" +
	        "--" + boundary + "\r\n" +
	        "Content-Disposition: form-data; name=\"build\"\r\n" +
	        "\r\n" +
	        JADED_BUILD_ID + "\r\n" +
	        "--" + boundary + "\r\n";

	/* Upload the crash dump and details. */

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	BOOL      bSuccess = FALSE;
	DWORD     dwStatusCode;

	hSession = WinHttpOpen( L"Jaded Crash Reporter/1.0",
	                        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
	                        WINHTTP_NO_PROXY_NAME,
	                        WINHTTP_NO_PROXY_BYPASS, 0 );

	if ( hSession )
	{
		hConnect = WinHttpConnect( hSession, JADED_CRASH_REPORT_HOST,
		                           INTERNET_DEFAULT_PORT, 0 );
	}

	if ( hConnect )
	{
		hRequest = WinHttpOpenRequest( hConnect, L"POST", JADED_CRASH_REPORT_PATH,
		                               NULL, WINHTTP_NO_REFERER,
		                               WINHTTP_DEFAULT_ACCEPT_TYPES,
		                               WINHTTP_FLAG_SECURE );
	}

	if ( hRequest )
	{
		bSuccess = WinHttpSendRequest( hRequest,
		                               ( L"Content-Type:multipart/form-data; boundary=" + wide_boundary ).c_str(), -1,
		                               ( void * ) ( request_body.data() ), request_body.size(),
		                               request_body.size(), 0 );
	}

	if ( bSuccess )
	{
		bSuccess = WinHttpReceiveResponse( hRequest, NULL );
	}

	if ( bSuccess )
	{
		DWORD dwSize = sizeof( dwStatusCode );

		bSuccess = WinHttpQueryHeaders( hRequest,
		                                WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		                                WINHTTP_HEADER_NAME_BY_INDEX,
		                                &dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX );
	}

	if ( hRequest ) WinHttpCloseHandle( hRequest );
	if ( hConnect ) WinHttpCloseHandle( hConnect );
	if ( hSession ) WinHttpCloseHandle( hSession );

	return bSuccess == TRUE && dwStatusCode == 201;
}

static int CALLBACK Win32CrashReporter( HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( iMsg )
	{
		case WM_INITDIALOG:
		{
			/* Center the crash reporter dialog on the primary display. */

			int pm_w = GetSystemMetrics( SM_CXSCREEN );
			int pm_h = GetSystemMetrics( SM_CYSCREEN );

			RECT r;
			GetWindowRect( hDlg, &r );

			int dlg_w = r.right - r.left + 1;
			int dlg_h = r.bottom - r.top + 1;

			MoveWindow( hDlg, ( ( pm_w / 2 ) - ( dlg_w / 2 ) ), ( ( pm_h / 2 ) - ( dlg_h / 2 ) ), dlg_w, dlg_h, FALSE );

			break;
		}

		case WM_COMMAND:
		{
			if ( wParam == IDOK )
			{
				HWND details_input = GetDlgItem( hDlg, IDC_EDIT2 );
				assert( details_input != NULL );

				std::vector< char > details_buf( Edit_GetTextLength( details_input ) + 1 );
				Edit_GetText( details_input, details_buf.data(), details_buf.size() );

				if ( Win32SendCrashReport( details_buf.data(), getenv( "JADED_CRASH_DUMP_PATH" ) ) )
				{
					MessageBox( hDlg, "Crash report submitted, thank you!", "Crash Report", MB_OK );
					EndDialog( hDlg, 0 );
				}
				else
				{
					MessageBox( hDlg, "Error sending crash report", "Crash Report", MB_OK );
				}
			}
			else if ( wParam == IDCANCEL )
			{
				EndDialog( hDlg, 0 );
			}

			break;
		}

		case WM_CLOSE:
		{
			EndDialog( hDlg, 0 );
			break;
		}

		default:
			return FALSE;
	}

	return TRUE;
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
		else if ( SDL_strcasecmp( jaded::sys::launchArguments[ i ], "/popupError" ) == 0 )// Showin added Param for PopUp Script Errors (if off it uses console)
		{
			jaded::sys::launchOperations.popupError = true;
			continue;
		}
		else if ( SDL_strcasecmp( jaded::sys::launchArguments[ i ], "/console" ) == 0 )
		{
			jaded::sys::launchOperations.debugConsole = true;
			continue;
		}
		else if ( SDL_strcasecmp( jaded::sys::launchArguments[ i ], "/window" ) == 0 )
		{
			jaded::sys::launchOperations.forceWindowed = true;
			continue;
		}
		else if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/width", 6 ) == 0 )
		{
			jaded::sys::launchOperations.forcedWidth = strtol( jaded::sys::launchArguments[ i ] + 7, nullptr, 10 );
			continue;
		}
		else if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/height", 7 ) == 0 )
		{
			jaded::sys::launchOperations.forcedHeight = strtol( jaded::sys::launchArguments[ i ] + 8, nullptr, 10 );
			continue;
		}
		else if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/profile", 8 ) == 0 )
		{
			jaded::sys::profiler.SetActive( true );
			ENG_gb_LimitFPS = false;
			continue;
		}
		else if ( SDL_strncasecmp( jaded::sys::launchArguments[ i ], "/bf", 3 ) == 0 )
		{
			if ( ++i >= jaded::sys::numLaunchArguments )
			{
				break;
			}

#if 0// init occurs after we parse launch arguments, and resets this...
			snprintf( MAI_gst_InitStruct.asz_ProjectName, sizeof( MAI_gst_InitStruct.asz_ProjectName ), "%s", jaded::sys::launchArguments[ i ] );
#else
			jaded::sys::launchOperations.projectFile = jaded::sys::launchArguments[ i ];
#endif
			continue;
		}
	}
}

static SDL_Window *CreateSDLWindow()
{
#if defined( USE_SDL_GL_CONTEXT )

	int flags = SDL_WINDOW_OPENGL;
	if ( !jaded::sys::launchOperations.forceWindowed )
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );

#else

	int flags = 0;

#endif

	int                    w, h;
	const SDL_DisplayMode *displayMode;
	if ( ( displayMode = SDL_GetDesktopDisplayMode( 0 ) ) != nullptr )
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

	if ( jaded::sys::launchOperations.forcedWidth > 0 ) w = jaded::sys::launchOperations.forcedWidth;
	if ( jaded::sys::launchOperations.forcedHeight > 0 ) h = jaded::sys::launchOperations.forcedHeight;

	sdlWindow = SDL_CreateWindow( "Jaded", w, h, flags );
	if ( sdlWindow == nullptr )
	{
		return nullptr;
	}

	if ( !jaded::sys::launchOperations.forceWindowed )
	{
		SDL_SetWindowFullscreen( sdlWindow, true );
	}

#if defined( USE_SDL_GL_CONTEXT )

	sdlGLContext = SDL_GL_CreateContext( sdlWindow );
	if ( sdlGLContext == nullptr )
		return nullptr;

	SDL_GL_MakeCurrent( sdlWindow, sdlGLContext );

#endif

#if defined( _WIN32 )

	nativeWindowHandle = ( HWND ) SDL_GetPointerProperty( SDL_GetWindowProperties( sdlWindow ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr );

#endif

	return sdlWindow;
}

static void SetupAspectRatio()
{
	int w, h;
	SDL_GetWindowSizeInPixels( sdlWindow, &w, &h );

	float r = ( ( float ) w / h );
	if ( fabsf( r - ( 4.0f / 3.0f ) ) < fabsf( r - ( 16.0f / 9.0f ) ) )
	{
		MAI_gst_MainHandles.pst_DisplayData->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_4over3;
	}
	else
	{
		MAI_gst_MainHandles.pst_DisplayData->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_16over9;
	}
}

static void InitializeDisplay()
{
	MAI_gh_MainWindow = MAI_gst_MainHandles.h_DisplayWindow = nativeWindowHandle;
	MAI_gst_MainHandles.pst_DisplayData                     = GDI_fnpst_CreateDisplayData();
	GDI_gpst_CurDD                                          = MAI_gst_MainHandles.pst_DisplayData;

	GDI_fnl_InitInterface( &MAI_gst_MainHandles.pst_DisplayData->st_GDI, 1 );

	MAI_gst_MainHandles.pst_DisplayData->pv_SpecificData = MAI_gst_MainHandles.pst_DisplayData->st_GDI.pfnpv_InitDisplay();
	GDI_AttachDisplay( MAI_gst_MainHandles.pst_DisplayData, MAI_gst_MainHandles.h_DisplayWindow );

#ifdef RASTERS_ON

	GDI_Rasters_Init( MAI_gst_MainHandles.pst_DisplayData->pst_Raster, "Display Data" );

#endif

	MAI_gst_MainHandles.pst_DisplayData->uc_EngineCamera = TRUE;
	MAI_gst_MainHandles.pst_DisplayData->ul_DrawMask |= GDI_Cul_DM_NoAutoClone;

	MAI_gst_MainHandles.pst_DisplayData->st_ScreenFormat.ul_Flags = GDI_Cul_SFF_OccupyAll;

	// Determine aspect ratio - TODO: should get triggered again whenever window-size changes...
	SetupAspectRatio();
}

static void ShutdownDisplay()
{
	if ( MAI_gst_MainHandles.pst_DisplayData == nullptr )
		return;

	GDI_DetachDisplay( MAI_gst_MainHandles.pst_DisplayData );
	MAI_gst_MainHandles.pst_DisplayData->st_GDI.pfnv_DesinitDisplay( MAI_gst_MainHandles.pst_DisplayData->pv_SpecificData );
	GDI_fnv_DestroyDisplayData( MAI_gst_MainHandles.pst_DisplayData );
}

#if defined( JADED_USE_WINMAIN_SDL )// hogsy: keep this for now, so if we need to, we can revert to the old crap

#	if defined( _WIN32 )
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow )
#	else
int main( int argc, char **argv )
#	endif
{
#	if defined( _WIN32 )

	const char *dump = getenv( "JADED_CRASH_DUMP_PATH" );
	if ( dump != NULL )
	{
		DialogBox( NULL, MAKEINTRESOURCE( DIALOGS_IDD_CRASH_REPORT ), NULL, &Win32CrashReporter );
		return EXIT_SUCCESS;
	}

	jaded::sys::numLaunchArguments = __argc;
	jaded::sys::launchArguments    = __argv;

	SetUnhandledExceptionFilter( Win32CrashHandler );

#	else

	jaded::sys::numLaunchArguments = argc;
	jaded::sys::launchArguments    = argv;

#	endif

	ParseStartupParameters();

	// start off with the working directory set to our launch location
	std::string exePath = jaded::filesystem.GetExecutablePath();
	size_t      p       = exePath.find_last_of( '/' );
	if ( p != std::string::npos )
	{
		exePath.erase( p );
	}
	jaded::filesystem.SetWorkingDirectory( exePath );

	if ( !SDL_Init( SDL_INIT_GAMEPAD | SDL_INIT_VIDEO ) )
	{
		jaded::sys::AlertBox( "SDL Init fail: " + std::string( SDL_GetError() ),
		                      "Jaded Error",
		                      jaded::sys::ALERT_BOX_ERROR );
		return EXIT_FAILURE;
	}

#	if defined( _WIN32 )

	if ( jaded::sys::launchOperations.debugConsole )
	{
		AllocConsole();
		FILE *tmp;
		freopen_s( &tmp, "CONIN$", "r", stdin );
		freopen_s( &tmp, "CONOUT$", "w", stderr );
		freopen_s( &tmp, "CONOUT$", "w", stdout );
	}

	// hogsy: for now we'll only support editor functionality under win32
	if ( jaded::sys::launchOperations.editorMode )
	{
		return EDI_EditorWin32Execution( hInstance );
	}

#	endif

	if ( CreateSDLWindow() == nullptr )
	{
		jaded::sys::AlertBox( "SDL Window fail: " + std::string( SDL_GetError() ),
		                      "Jaded Error",
		                      jaded::sys::ALERT_BOX_ERROR );
	}

	ImGuiInterface_Initialize( sdlWindow );

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
	if ( !jaded::filesystem.SetProject( MAI_gst_InitStruct.asz_ProjectName ) )
	{
		std::string msg = "Failed to set project\n" + std::string( MAI_gst_InitStruct.asz_ProjectName ) + "\n\nPlease check the logs for more details.";
		jaded::sys::AlertBox( msg.c_str(), "Jaded", jaded::sys::ALERT_BOX_ERROR );
		return EXIT_FAILURE;
	}

	InitializeDisplay();

	ENG_InitEngine();

	ENG_Loop();

	WOR_Universe_Close( 0 );

	ENG_CloseEngine();
	ENG_CloseApplication();

	ImGuiInterface_Shutdown();

	SDL_DestroyWindow( sdlWindow );

#	if defined( _WIN32 ) && !defined( NDEBUG )

	if ( jaded::sys::launchOperations.debugConsole )
		FreeConsole();

#	endif

	return EXIT_SUCCESS;
}

#endif
