/*$T BAStypes.h GC! 1.077 03/24/00 14:11:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS
#	ifdef _DEBUG
#		define AI_FULL_RASTERS
#	endif// _DEBUG
#endif    // ACTIVE_EDITORS

#include <stdint.h>
#include <stdbool.h>

#if defined( __cplusplus )
#	include <cassert>
#	include <array>
#	include <string>
#	include <vector>
#	include <map>
#else
#	include <assert.h>
#endif

#define BAS_ARRAY_ELEMENTS( a )  ( sizeof( a ) / sizeof( *( a ) ) )// Returns the number of elements within an array.
#define BAS_MAX_ARRAY_INDEX( a ) ( int ) ( BAS_ARRAY_ELEMENTS( a ) - 1 )

#define BAS_FILENAME ( strrchr( __FILE__, '\\' ) ? strrchr( __FILE__, '\\' ) + 1 : __FILE__ )

#define BAS_ZERO( DATA, SIZE ) memset( ( DATA ), 0, ( SIZE ) )

#define BAS_BITFLAG( A, B ) A = ( 1U << B )

//###################################################################################
//###################################################################################
#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
//###################################################################################
//###################################################################################

#	define ONLY_PSX2_ALIGNED( a )
#	define ONLY_XBOX_ALIGNED( a )

#	ifndef NOMINMAX
#		ifndef max
#			define max( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#		endif
#		ifndef min
#			define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#		endif
#	endif

//###################################################################################
//###################################################################################
// END OF #elif defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
#endif /* platform selection */

/*$4
 ***********************************************************************************************************************
    Other useful types.
 ***********************************************************************************************************************
 */

#ifdef FALSE
#	undef FALSE
#endif
#define FALSE false
#ifdef TRUE
#	undef TRUE
#endif
#define TRUE true

#if defined( NULL )
#	undef NULL
#endif
#define NULL 0// I thought this was already done somewhere else O:-)

// hogsy: !!TODO!! we should eventually get rid of the below and use stdint types instead

typedef char CHAR;
typedef long LONG; /* specific */
typedef unsigned long ULONG;
typedef int BOOL;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;
