// File created for Jaded, the community patched Jade engine

#pragma once

#ifdef __cplusplus
#	include <cstdint>
#	include <cassert>

#	include <stdexcept>

#	include <algorithm>
#	include <string>
#	include <vector>
#	include <map>
#endif

/////////////////////////////////////////////////////////////////////////////////////
// System Macros
/////////////////////////////////////////////////////////////////////////////////////

#define CORE_SYSTEM_WINDOWS 1
#define CORE_SYSTEM_LINUX   2
#define CORE_SYSTEM_MACOS   3

#if defined( __linux__ )
#	include <signal.h>

#	define CORE_SYSTEM_NAME    "Linux"
#	define CORE_SYSTEM         CORE_SYSTEM_LINUX
#	define CORE_SYSTEM_EXE_EXT ""
#	define CORE_SYSTEM_LIB_EXT ".so"

#	define CORE_SYSTEM_IMPLEMENT_MAIN() \
		int main( int argc, char **argv ) { return core_main( argc, argv ); }
#elif defined( __APPLE__ )
#	define CORE_SYSTEM_NAME    "macOS"
#	define CORE_SYSTEM         CORE_SYSTEM_MACOS
#	define CORE_SYSTEM_EXE_EXT ""
#	define CORE_SYSTEM_LIB_EXT ".so"

#	define CORE_SYSTEM_IMPLEMENT_MAIN() \
		int main( int argc, char **argv ) { return core_main( argc, argv ); }
#elif defined( _WIN32 )
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	define CORE_SYSTEM_NAME    "Windows"
#	define CORE_SYSTEM         CORE_SYSTEM_WINDOWS
#	define CORE_SYSTEM_EXE_EXT ".exe"
#	define CORE_SYSTEM_LIB_EXT ".dll"

#	define CORE_SYSTEM_IMPLEMENT_MAIN() \
		int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw ) { return core_main( __argc, __argv ); }
#else
#	error "Unknown target system!"
#endif

#if !defined( CORE_SYSTEM_NAME ) || !defined( CORE_SYSTEM ) || !defined( CORE_SYSTEM_EXE_EXT ) || !defined( CORE_SYSTEM_LIB_EXT )
#	error "Required macros haven't been declared for the target system!"
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Hardware Macros
/////////////////////////////////////////////////////////////////////////////////////

// Endiannes
enum
{
	CORE_HARDWARE_LITTLE_ENDIAN,
	CORE_HARDWARE_BIG_ENDIAN,
};

// Architecture
enum
{
	CORE_HARDWARE_CPU_X86,
	CORE_HARDWARE_CPU_X64,
	CORE_HARDWARE_CPU_ARM,
	CORE_HARDWARE_CPU_ARM64,
};

#if defined( __amd64 ) || defined( __amd64__ ) || defined( _M_X64 ) || defined( _M_AMD64 )
#	define CORE_HARDWARE_CPU        CORE_HARDWARE_CPU_X64
#	define CORE_HARDWARE_CPU_NAME   "AMD64"
#	define CORE_HARDWARE_ENDIANNESS CORE_HARDWARE_LITTLE_ENDIAN
#elif defined( __arm__ ) || defined( __thumb__ ) || defined( _M_ARM ) || defined( _M_ARMT )
#	define CORE_HARDWARE_CPU        CORE_HARDWARE_CPU_ARM
#	define CORE_HARDWARE_CPU_NAME   "ARM"
#	define CORE_HARDWARE_ENDIANNESS CORE_HARDWARE_LITTLE_ENDIAN
#elif defined( __aarch64__ )
#	define CORE_HARDWARE_CPU        CORE_HARDWARE_CPU_ARM64
#	define CORE_HARDWARE_CPU_NAME   "ARM64"
#	define CORE_HARDWARE_ENDIANNESS CORE_HARDWARE_LITTLE_ENDIAN
#elif defined( i386 ) || defined( __i386 ) || defined( __i386__ ) || defined( _M_I86 ) || defined( _M_IX86 ) || defined( _X86_ )
#	define CORE_HARDWARE_CPU        CORE_HARDWARE_CPU_X86
#	define CORE_HARDWARE_CPU_NAME   "INTEL86"
#	define CORE_HARDWARE_ENDIANNESS CORE_HARDWARE_LITTLE_ENDIAN
#else
#	error "Unsupported CPU type!"
#endif

#if !defined( CORE_HARDWARE_CPU ) || !defined( CORE_HARDWARE_CPU_NAME ) || !defined( CORE_HARDWARE_ENDIANNESS )
#	error "Required macros haven't been declared for the target hardware!"
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Compiler Macros
/////////////////////////////////////////////////////////////////////////////////////

#if defined( __GNUC__ ) || defined( __GNUG__ )
#	define CORE_IMPORT
#	define CORE_EXPORT __attribute__( ( visibility( "default" ) ) )
#elif defined( _MSC_VER )
#	define CORE_IMPORT
#	define CORE_EXPORT __declspec( dllexport )
#else
#	error "Unsupported compiler!"
#endif

#ifdef __cplusplus
#	define CORE_EXTERN_C \
		extern "C"        \
		{
#	define CORE_EXTERN_C_END }
#else
#	define CORE_EXTERN_C
#	define CORE_EXTERN_C_END
#endif

/////////////////////////////////////////////////////////////////////////////////////
// Helper Macros
/////////////////////////////////////////////////////////////////////////////////////

// horrible gross disgusting poopy windows types :(
#if CORE_SYSTEM != CORE_SYSTEM_WINDOWS
typedef char           CHAR;
typedef unsigned char  UCHAR;
typedef short          SHORT;
typedef unsigned short USHORT;
#endif

#define CORE_ZERO( DATA, SIZE ) memset( ( DATA ), 0, ( SIZE ) )
#define CORE_ZERO_( DATA )      memset( &( DATA ), 0, sizeof( ( DATA ) ) )

#define CORE_SET_ARRAY( DATA, VALUE, ELEMENTS ) memset( ( DATA ), ( VALUE ), sizeof( typeof( *( DATA ) ) ) * ( ELEMENTS ) )
#define CORE_ZERO_ARRAY( DATA, ELEMENTS )       CORE_SET_ARRAY( DATA, 0, ELEMENTS )

#define CORE_STRINGIFY( NUM ) #NUM
#define CORE_TO_STRING( A )   CORE_STRINGIFY( A )

#define CORE_ARRAY_ELEMENTS( A )  ( sizeof( A ) / sizeof( *( A ) ) )
#define CORE_MAX_ARRAY_INDEX( A ) ( int ) ( CORE_ARRAY_ELEMENTS( A ) - 1 )

#define CORE_MIN( A, B ) ( ( A ) < ( B ) ? ( A ) : ( B ) )
#define CORE_MAX( A, B ) ( ( A ) > ( B ) ? ( A ) : ( B ) )

#define CORE_BIT_FLAG( A, B ) A = ( 1U << ( B ) )

#define CORE_MAGIC_TO_NUM( A, B, C, D ) ( ( ( D ) << 24 ) + ( ( C ) << 16 ) + ( ( B ) << 8 ) + ( A ) )

#define CORE_FLOAT_TO_BYTE( A ) ( unsigned char ) ( roundf( ( A ) * 255.0f ) )
#define CORE_BYTE_TO_FLOAT( A ) ( ( A ) / ( float ) 255 )

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
