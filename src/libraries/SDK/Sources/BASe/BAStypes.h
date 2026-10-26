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

#define BAS_FILENAME ( strrchr( __FILE__, '\\' ) ? strrchr( __FILE__, '\\' ) + 1 : __FILE__ )

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
